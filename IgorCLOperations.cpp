//
//  IgorCLOperations.cpp
//  IgorCL
//
//  Created by Peter Dedecker on 05/02/2013.
//
//

#include "IgorCLOperations.h"

#include <fstream>

#include "IgorCLUtilities.h"
#include "IgorCLConstants.h"

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::string* sourceText, const std::vector<char>* sourceBinary);

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::string& sourceText) {
    DoOpenCLCalculation(platformIndex, deviceIndex, globalRange, workgroupSize, kernelName, waves, memFlags, &sourceText, NULL);
}

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::vector<char>& sourceBinary) {
    DoOpenCLCalculation(platformIndex, deviceIndex, globalRange, workgroupSize, kernelName, waves, memFlags, NULL, &sourceBinary);
}

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::string* sourceText, const std::vector<char>* sourceBinary) {
    
    size_t nWaves = waves.size();
    
    // vectors that will hold a pointer to the data and the size of the data
    std::vector<void*> dataPointers; std::vector<size_t> dataSizes;
    dataPointers.reserve(nWaves); dataSizes.reserve(nWaves);
    for (size_t i = 0; i < nWaves; i+=1) {
        // special case: if we're using __shared memory then the corresponding wave must
        // consist of a single point, the size of the memory.
        if ((memFlags.size() > i) && (memFlags.at(i) & IgorCLIsLocalMemory)) {
            dataPointers.push_back(NULL);
            dataSizes.push_back(SharedMemorySizeFromWave(waves.at(i)));
        } else {
            dataPointers.push_back(reinterpret_cast<void*>(WaveData(waves.at(i))));
            dataSizes.push_back(WaveDataSizeInBytes(waves.at(i)));
        }
    }
    
    // convert IgorCL memflags to underlying OpenCL flags
    std::vector<int> openCLMemFlags;
    for (int i = 0; i < memFlags.size(); i+=1) {
        int clFlags = ConvertIgorCLFlagsToOpenCLFlags(memFlags.at(i));
        openCLMemFlags.push_back(clFlags);
    }
    
    // obtain the appropriate context and device.
    cl::Context context;
    cl::Device device;
    contextAndDeviceProvider.getContextForPlatformAndDevice(platformIndex, deviceIndex, context, device);
    std::vector<cl::Device> deviceAsVector(1, device);
    
    // fetch a queue on the platform/device combination
    cl_int status;
    cl::CommandQueue commandQueue(context, device, 0, &status);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    // get the program, either using text or using source
    cl::Program program;
    if (sourceText != NULL) {
        // use text source
        program = cl::Program(context, *sourceText, &status);
    } else {
        // use binary
        const void* programPointer = &(sourceBinary->at(0));
        size_t programSize = sourceBinary->size();
        std::pair<const void*, size_t> sourcePair(programPointer, programSize);
        std::vector<std::pair<const void*, size_t> > binaryAsVector;
        binaryAsVector.push_back(sourcePair);
        program = cl::Program(context, deviceAsVector, binaryAsVector, NULL, &status);
    }
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    // build the program
    status = program.build("-cl-mad-enable");
    if (status != CL_SUCCESS) {
        std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        for (int i = 0; i < buildLog.size(); ++i) {
            if (buildLog[i] == '\n')
                buildLog[i] = '\r';
        }
        XOPNotice(buildLog.c_str());
        throw IgorCLError(status);
    }
    
    // fetch the kernel
    cl::Kernel kernel(program, kernelName.c_str(), &status);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    // create buffers for all of the input data
    std::vector<cl::Buffer> buffers;
    buffers.reserve(nWaves);
    for (size_t i = 0; i < nWaves; i+=1) {
        if ((memFlags.size() > i) && (memFlags.at(i) & (IgorCLIsLocalMemory | IgorCLIsScalarArgument))) {
            buffers.push_back(cl::Buffer());
            continue;
        }
        
        int flags = 0;
        void* hostPointer = NULL;
        if (openCLMemFlags.size() > i)
            flags = openCLMemFlags.at(i);
        if (flags & CL_MEM_USE_HOST_PTR)
            hostPointer = dataPointers.at(i);
        cl::Buffer buffer(context, flags, dataSizes.at(i), hostPointer, &status);
        if (status != CL_SUCCESS)
            throw IgorCLError(status);
        buffers.push_back(buffer);
    }
    
    // and copy all of the data to the device, unless we want to use the host memory, we're using shared memory, this is a scalar argument,
    // or this memory is write-only.
    for (size_t i = 0; i < nWaves; i+=1) {
        if ((memFlags.size() > i) && (memFlags.at(i) & (IgorCLIsLocalMemory | IgorCLIsScalarArgument)))
            continue;
        if ((openCLMemFlags.size() > i) && (openCLMemFlags.at(i) & (CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY)))
            continue;
        status = commandQueue.enqueueWriteBuffer(buffers.at(i), false, 0, dataSizes.at(i), dataPointers.at(i));
        if (status != CL_SUCCESS)
            throw IgorCLError(status);
    }
    
    // set arguments for the kernel
    for (size_t i = 0; i < nWaves; i+=1) {
        if ((memFlags.size() > i) && (memFlags.at(i) & IgorCLIsLocalMemory)) {
            status = kernel.setArg(i, dataSizes.at(i), NULL);
        } else if ((memFlags.size() > i) && (memFlags.at(i) & IgorCLIsScalarArgument)) {
            status = kernel.setArg(i, dataSizes.at(i), dataPointers.at(i));
        } else {
            status = kernel.setArg(i, buffers.at(i));
        }
        if (status != CL_SUCCESS)
            throw IgorCLError(status);
    }
    
    // perform the actual calculation
    status = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, globalRange, workgroupSize, NULL, NULL);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    // copy arguments back into the waves, unless we have used host memory, used shared memory, this is a scalar argument,
    // or this memory is read-only.
    for (size_t i = 0; i < nWaves; i+=1) {
        if ((memFlags.size() > i) && (memFlags.at(i) & (IgorCLIsLocalMemory | IgorCLIsScalarArgument)))
            continue;
        if ((openCLMemFlags.size() > i) && (openCLMemFlags.at(i) & (CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY)))
            continue;
        status = commandQueue.enqueueReadBuffer(buffers.at(i), false, 0, dataSizes.at(i), dataPointers.at(i));
        if (status != CL_SUCCESS)
            throw IgorCLError(status);
    }
    
    // block until everything is finished
    status = commandQueue.finish();
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
}

std::vector<char> CompileSource(const int platformIndex, const int deviceIndex, const std::string programSource, std::string& buildLog) {
    // initialize the platforms and devices
    cl_int status;
    std::vector<cl::Platform> platforms;
    status = cl::Platform::get(&platforms);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    cl::Platform platform = platforms.at(platformIndex);
    
    std::vector<cl::Device> devices;
    status = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    cl::Device device = devices.at(deviceIndex);
    
    // initialize the context
    std::vector<cl::Device> deviceAsVector;
    deviceAsVector.push_back(device);
    cl::Context context(deviceAsVector, NULL, NULL, NULL, &status);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    // initialize the program and have it build the source code
    cl::Program program(context, programSource, &status);
    if (status != CL_SUCCESS)
        throw IgorCLError(status);
    
    // build the program
    buildLog.clear();
    status = program.build("-cl-mad-enable");
    buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
    for (int i = 0; i < buildLog.size(); ++i) {
        if (buildLog[i] == '\n')
            buildLog[i] = '\r';
    }
    if (status != CL_SUCCESS) {
        throw IgorCLError(status);
    }
    
    std::vector<char*> programBinary;
    std::vector<size_t> programBinarySizes;
    programBinary = program.getInfo<CL_PROGRAM_BINARIES>();
    programBinarySizes = program.getInfo<CL_PROGRAM_BINARY_SIZES>();
    
    std::vector<char> compiledBinary;
    compiledBinary.resize(programBinarySizes.at(0));
    memcpy(reinterpret_cast<void*>(&compiledBinary.at(0)), reinterpret_cast<void*>(programBinary.at(0)), programBinarySizes.at(0));
    
    return compiledBinary;
}
