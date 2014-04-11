//
//  IgorCLUtilities.cpp
//  IgorCL
//
//  Created by Peter Dedecker on 06/02/2013.
//
//

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
