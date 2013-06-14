//
//  IgorCLUtilities.cpp
//  IgorCL
//
//  Created by Peter Dedecker on 06/02/2013.
//
//

#include <stdexcept>
#include <string>

#include "IgorCLUtilities.h"
#include "IgorCLConstants.h"
#include "cl.hpp"

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
    boost::scoped_array<char> cString(new char[strLength + 1]);
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
    
    switch (WaveType(wave)) {
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
    
    if (igorCLFlags & IgorCLReadWrite)
        openCLFlags |= CL_MEM_READ_WRITE;
    if (igorCLFlags & IgorCLWriteOnly)
        openCLFlags |= CL_MEM_WRITE_ONLY;
    if (igorCLFlags & IgorCLReadOnly)
        openCLFlags |= CL_MEM_READ_ONLY;
    if (igorCLFlags & IgorCLUseHostPointer)
        openCLFlags |= CL_MEM_USE_HOST_PTR;
    
    // this function does not bother with IgorCLIsLocalMemory or IgorCLIsScalarArgument
    // because there are no corresponding OpenCL flags.
    
    return openCLFlags;
}
