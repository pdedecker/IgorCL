/*
IgorCL - an XOP to use OpenCL in Igor Pro
Copyright(C) 2013-2017 Peter Dedecker

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

The developer(s) of this software hereby grants permission to link
this program with Igor Pro, developed by WaveMetrics Inc. (www.wavemetrics.com).
*/

#include <stdexcept>
#include <string>
#include <cctype>
#include <memory>

#include "IgorCLUtilities.h"
#include "IgorCLConstants.h"

void StoreStringInTextWave(const std::string str, waveHndl textWave, IndexInt* indices) {
    int err;
    
    Handle textHandle = PutStdStringInHandle(str);
    
    err = MDSetTextWavePointValue(textWave, indices, textHandle);
    if (err)
        throw int(err);
    
    DisposeHandle(textHandle);
}

std::string GetStdStringFromHandle(const Handle handle) {
    int err;
    int strLength = GetHandleSize(handle);
    std::unique_ptr<char[]> cString(new char[strLength + 1]);
    err = GetCStringFromHandle(handle, cString.get(), strLength);
    if (err)
        throw int(err);
    
    return std::string(cString.get());
}

Handle PutStdStringInHandle(const std::string theString) {
    int strLength = theString.length();
    
    Handle textHandle = NewHandle(strLength);
    if (textHandle == NULL)
        throw std::bad_alloc();
    
    int err = PutCStringInHandle(theString.c_str(), textHandle);
    if (err)
        throw int(err);
    return textHandle;
}

size_t WaveDataSizeInBytes(waveHndl wave) {
    int err = 0;
    size_t dataSize;
    size_t bytesPerPoint;
    
    int waveType = WaveType(wave);
    if (waveType & NT_UNSIGNED)
        waveType &= ~NT_UNSIGNED;   // remove unsigned flag - does not affect data size
    if (waveType & NT_CMPLX)
        waveType &= ~NT_CMPLX;      // remove complex flag - we will take care of this later
    
    switch (waveType) {
        case NT_I8:
            bytesPerPoint = 1;
            break;
        case NT_I16:
            bytesPerPoint = 2;
            break;
        case NT_I32:
            bytesPerPoint = 4;
            break;
        case NT_FP32:
            bytesPerPoint = 4;
            break;
        case NT_FP64:
            bytesPerPoint = 8;
            break;
        default:
            throw int(NOMEM);
            break;
    }
    
    if (WaveType(wave) & NT_CMPLX)
        bytesPerPoint *= 2;
    
    return WavePoints(wave) * bytesPerPoint;
}

size_t SharedMemorySizeFromWave(waveHndl wave) {
    int err = 0;
    
    if ((WavePoints(wave) != 1) || (WaveType(wave) & NT_CMPLX))
        throw std::runtime_error("Size of __shared memory must be specified in a numeric, non-complex wave with a single point");
    
    double dMemorySize;
    err = MDGetDPDataFromNumericWave(wave, &dMemorySize);
    if (err)
        throw err;
    
    return static_cast<size_t>(dMemorySize);
}

int GetFirstDeviceOfType(const int platformIndex, const std::string& deviceTypeStr) {
    std::string upperCaseStr(deviceTypeStr);
    for (int i = 0; i < upperCaseStr.size(); ++i) {
        upperCaseStr[i] = std::toupper(upperCaseStr[i]);
    }
    
    int deviceType;
    if (upperCaseStr == "CPU") {
        deviceType = CL_DEVICE_TYPE_CPU;
    } else if (upperCaseStr == "GPU") {
        deviceType = CL_DEVICE_TYPE_GPU;
    } else if (upperCaseStr == "ACCELERATOR") {
        deviceType = CL_DEVICE_TYPE_ACCELERATOR;
    } else {
        throw std::runtime_error("Unknown device type string");
    }
    
    // initialize the platform
    cl_int status;
    std::vector<cl::Platform> platforms;
    status = cl::Platform::get(&platforms);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    if (platforms.size() <= platformIndex)
        throw std::runtime_error("Invalid OpenCL platform index");
    cl::Platform platform = platforms.at(platformIndex);
    
    std::vector<cl::Device> devices;
    status = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    for (int i = 0; i < devices.size(); ++i) {
        cl_device_type thisDeviceType = devices.at(i).getInfo<CL_DEVICE_TYPE>();
        if (thisDeviceType == deviceType)
            return i;
    }
    
    // still here? No matching device.
    throw std::runtime_error("No device of requested type available");
}

int ConvertIgorCLFlagsToOpenCLFlags(const int igorCLFlags) {
    int openCLFlags = 0;
    
    // some error checking.
    if ((igorCLFlags & IgorCLWriteOnly) && (igorCLFlags & IgorCLReadOnly)) {
        throw int(INCOMPATIBLE_FLAGS);
    }
    if ((igorCLFlags & IgorCLUsePinnedMemory) && (igorCLFlags & IgorCLUseHostPointer)) {
        throw int(INCOMPATIBLE_FLAGS);
    }
    
    // convert all IgorCL flags for which there is an equivalent OpenCL flag.
    if (igorCLFlags & IgorCLReadWrite)
        openCLFlags |= CL_MEM_READ_WRITE;
    if (igorCLFlags & IgorCLWriteOnly)
        openCLFlags |= CL_MEM_WRITE_ONLY;
    if (igorCLFlags & IgorCLReadOnly)
        openCLFlags |= CL_MEM_READ_ONLY;
    if (igorCLFlags & IgorCLUseHostPointer)
        openCLFlags |= CL_MEM_USE_HOST_PTR;
    
    return openCLFlags;
}

void IgorCLContextAndDeviceProvider::getContextForPlatformAndDevice(const int platformIndex, const int deviceIndex, cl::Context& context, cl::Device &device) {
    std::lock_guard<std::mutex> lock(this->contextMutex);
    
    // check if we already have a context for this combination.
    std::pair<int, int> requestedIndices(platformIndex, deviceIndex);
    for (int i = 0; i < availableContextIndices.size(); ++i) {
        if (availableContextIndices[i] == requestedIndices) {
            context = availableContexts.at(i);
            device = deviceForContext.at(i);
            return;
        }
    }
    
    // if we are still here then the context needs to be created.
    // initialize the platforms
    cl_int status;
    std::vector<cl::Platform> platforms;
    status = cl::Platform::get(&platforms);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    cl::Platform platform = platforms.at(platformIndex);
    
    // fetch the device
    std::vector<cl::Device> devices;
    status = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    if (devices.size() <= deviceIndex)
        throw IgorCLError(CL_DEVICE_NOT_FOUND);
    device = devices.at(deviceIndex);
    
    // initialize the context
    std::vector<cl::Device> deviceAsVector;
    deviceAsVector.push_back(device);
    context = cl::Context(deviceAsVector, NULL, NULL, NULL, &status);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    availableContextIndices.push_back(requestedIndices);
    availableContexts.push_back(context);
    deviceForContext.push_back(device);
    
    return;
}

IgorCLContextAndDeviceProvider contextAndDeviceProvider;

cl::CommandQueue IgorCLCommandQueueFactory::getCommandQueue(const int platformIndex, const int deviceIndex) {
    std::lock_guard<std::mutex> lock(this->_queueMutex);
    
    // do we have a queue available?
    bool haveCacheForMatchingQueues = false;
    std::pair<int, int> requestedIndices(platformIndex, deviceIndex);
    for (int i = 0; i < _availableQueueIndices.size(); ++i) {
        if (_availableQueueIndices[i] == requestedIndices) {
            haveCacheForMatchingQueues = true;
            std::vector<cl::CommandQueue>* matchingQueues = &_availableQueues.at(i);
            int nMatchingQueues = matchingQueues->size();
            if (nMatchingQueues > 0) {
                cl::CommandQueue theQueue = matchingQueues->at(nMatchingQueues - 1);
                matchingQueues->erase(matchingQueues->end() - 1);
                return theQueue;
            }
        }
    }
    
    // if we're still here then we need to add a new queue and return that.
    cl::Context context;
    cl::Device device;
    contextAndDeviceProvider.getContextForPlatformAndDevice(platformIndex, deviceIndex, context, device);
    cl_int status;
    cl::CommandQueue commandQueue(context, device, 0, &status);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    if (!haveCacheForMatchingQueues) {
        // add storage for this platform/device combination,
        // but do not store the queue itself since it is in use and therefore unavailable.
        _availableQueueIndices.push_back(requestedIndices);
        _availableQueues.push_back(std::vector<cl::CommandQueue>());
    }
    
    return commandQueue;
}

void IgorCLCommandQueueFactory::returnCommandQueue(const cl::CommandQueue commandQueue, const int platformIndex, const int deviceIndex) {
    std::lock_guard<std::mutex> lock(this->_queueMutex);
    
    std::pair<int, int> requestedIndices(platformIndex, deviceIndex);
    for (int i = 0; i < _availableQueueIndices.size(); ++i) {
        if (_availableQueueIndices[i] == requestedIndices) {
            std::vector<cl::CommandQueue>* matchingQueues = &_availableQueues.at(i);
            matchingQueues->push_back(commandQueue);
            return;
        }
    }
    
    // still here? Shouldn't happen.
    throw std::logic_error("Returning command queue but no matching storage");
}

void IgorCLCommandQueueFactory::deleteAllCommandQueues() {
    std::lock_guard<std::mutex> lock(_queueMutex);
    
    _availableQueueIndices.clear();
    _availableQueues.clear();
    
}

IgorCLCommandQueueFactory commandQueueFactory;

IgorCLCommandQueueProvider::IgorCLCommandQueueProvider(const int platformIndex, const int deviceIndex) :
    _platformIndex(platformIndex),
    _deviceIndex(deviceIndex)
{
    _commandQueue = commandQueueFactory.getCommandQueue(platformIndex, deviceIndex);
}

IgorCLCommandQueueProvider::~IgorCLCommandQueueProvider() {
    commandQueueFactory.returnCommandQueue(_commandQueue, _platformIndex, _deviceIndex);
}

std::string OpenCLErrorCodeToSymbolicName(int errorCode) {
    switch (errorCode) {
        case 0:
            return std::string("CL_SUCCESS");
            break;
        case -1:
            return std::string("CL_DEVICE_NOT_FOUND");
            break;
        case -2:
            return std::string("CL_DEVICE_NOT_AVAILABLE");
            break;
        case -3:
            return std::string("CL_COMPILER_NOT_AVAILABLE");
            break;
        case -4:
            return std::string("CL_MEM_OBJECT_ALLOCATION_FAILURE");
            break;
        case -5:
            return std::string("CL_OUT_OF_RESOURCES");
            break;
        case -6:
            return std::string("CL_OUT_OF_HOST_MEMORY");
            break;
        case -7:
            return std::string("CL_PROFILING_INFO_NOT_AVAILABLE");
            break;
        case -8:
            return std::string("CL_MEM_COPY_OVERLAP");
            break;
        case -9:
            return std::string("CL_IMAGE_FORMAT_MISMATCH");
            break;
        case -10:
            return std::string("CL_IMAGE_FORMAT_NOT_SUPPORTED");
            break;
        case -11:
            return std::string("CL_BUILD_PROGRAM_FAILURE");
            break;
        case -12:
            return std::string("CL_MAP_FAILURE");
            break;
        case -13:
            return std::string("CL_MISALIGNED_SUB_BUFFER_OFFSET");
            break;
        case -14:
            return std::string("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST");
            break;
        case -15:
            return std::string("CL_COMPILE_PROGRAM_FAILURE");
            break;
        case -16:
            return std::string("CL_LINKER_NOT_AVAILABLE");
            break;
        case -17:
            return std::string("CL_LINK_PROGRAM_FAILURE");
            break;
        case -18:
            return std::string("CL_DEVICE_PARTITION_FAILED");
            break;
        case -19:
            return std::string("CL_KERNEL_ARG_INFO_NOT_AVAILABLE");
            break;
        case -30:
            return std::string("CL_INVALID_VALUE");
            break;
        case -31:
            return std::string("CL_INVALID_DEVICE_TYPE");
            break;
        case -32:
            return std::string("CL_INVALID_PLATFORM");
            break;
        case -33:
            return std::string("CL_INVALID_DEVICE");
            break;
        case -34:
            return std::string("CL_INVALID_CONTEXT");
            break;
        case -35:
            return std::string("CL_INVALID_QUEUE_PROPERTIES");
            break;
        case -36:
            return std::string("CL_INVALID_COMMAND_QUEUE");
            break;
        case -37:
            return std::string("CL_INVALID_HOST_PTR");
            break;
        case -38:
            return std::string("CL_INVALID_MEM_OBJECT");
            break;
        case -39:
            return std::string("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR");
            break;
        case -40:
            return std::string("CL_INVALID_IMAGE_SIZE");
            break;
        case -41:
            return std::string("CL_INVALID_SAMPLER");
            break;
        case -42:
            return std::string("CL_INVALID_BINARY");
            break;
        case -43:
            return std::string("CL_INVALID_BUILD_OPTIONS");
            break;
        case -44:
            return std::string("CL_INVALID_PROGRAM");
            break;
        case -45:
            return std::string("CL_INVALID_PROGRAM_EXECUTABLE");
            break;
        case -46:
            return std::string("CL_INVALID_KERNEL_NAME");
            break;
        case -47:
            return std::string("CL_INVALID_KERNEL_DEFINITION");
            break;
        case -48:
            return std::string("CL_INVALID_KERNEL");
            break;
        case -49:
            return std::string("CL_INVALID_ARG_INDEX");
            break;
        case -50:
            return std::string("CL_INVALID_ARG_VALUE");
            break;
        case -51:
            return std::string("CL_INVALID_ARG_SIZE");
            break;
        case -52:
            return std::string("CL_INVALID_KERNEL_ARGS");
            break;
        case -53:
            return std::string("CL_INVALID_WORK_DIMENSION");
            break;
        case -54:
            return std::string("CL_INVALID_WORK_GROUP_SIZE");
            break;
        case -55:
            return std::string("CL_INVALID_WORK_ITEM_SIZE");
            break;
        case -56:
            return std::string("CL_INVALID_GLOBAL_OFFSET");
            break;
        case -57:
            return std::string("CL_INVALID_EVENT_WAIT_LIST");
            break;
        case -58:
            return std::string("CL_INVALID_EVENT");
            break;
        case -59:
            return std::string("CL_INVALID_OPERATION");
            break;
        case -60:
            return std::string("CL_INVALID_GL_OBJECT");
            break;
        case -61:
            return std::string("CL_INVALID_BUFFER_SIZE");
            break;
        case -62:
            return std::string("CL_INVALID_MIP_LEVEL");
            break;
        case -63:
            return std::string("CL_INVALID_GLOBAL_WORK_SIZE");
            break;
        case -64:
            return std::string("CL_INVALID_PROPERTY");
            break;
        case -65:
            return std::string("CL_INVALID_IMAGE_DESCRIPTOR");
            break;
        case -66:
            return std::string("CL_INVALID_COMPILER_OPTIONS");
            break;
        case -67:
            return std::string("CL_INVALID_LINKER_OPTIONS");
            break;
        case -68:
            return std::string("CL_INVALID_DEVICE_PARTITION_COUNT");
            break;
        default:
            return std::string("unknown error code");
            break;
    }
}
