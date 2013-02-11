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

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::string* sourceText, const std::vector<char>* sourceBinary);

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::string& sourceText) {
    DoOpenCLCalculation(platformIndex, deviceIndex, globalRange, workgroupSize, kernelName, waves, &sourceText, NULL);
}

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<char>& sourceBinary) {
    DoOpenCLCalculation(platformIndex, deviceIndex, globalRange, workgroupSize, kernelName, waves, NULL, &sourceBinary);
}

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::string* sourceText, const std::vector<char>* sourceBinary) {
    
    size_t nWaves = waves.size();
    
    // vectors that will hold a pointer to the data and the size of the data
    std::vector<void*> dataPointers; std::vector<size_t> dataSizes;
    dataPointers.reserve(nWaves); dataSizes.reserve(nWaves);
    for (size_t i = 0; i < nWaves; i+=1) {
        dataPointers.push_back(reinterpret_cast<void*>(WaveData(waves.at(i))));
        dataSizes.push_back(WaveDataSizeInBytes(waves.at(i)));
    }
    
    // initialize the platforms and devices
    cl_int status;
    std::vector<cl::Platform> platforms;
    status = cl::Platform::get(&platforms);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    cl::Platform platform = platforms.at(platformIndex);
    
    std::vector<cl::Device> devices;
    status = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    cl::Device device = devices.at(deviceIndex);
    
    // initialize the context
    std::vector<cl::Device> deviceAsVector;
    deviceAsVector.push_back(device);
    cl::Context context(deviceAsVector, NULL, NULL, NULL, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    // fetch a queue on the platform/device combination
    cl::CommandQueue commandQueue(context, device, 0, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
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
        throw int(NOMEM);
    
    // build the program
    status = program.build();
    if (status != CL_SUCCESS) {
        std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        for (int i = 0; i < buildLog.size(); ++i) {
            if (buildLog[i] == '\n')
                buildLog[i] = '\r';
        }
        XOPNotice(buildLog.c_str());
        throw int(NOMEM);
    }
    
    // fetch the kernel
    cl::Kernel kernel(program, kernelName.c_str(), &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    // create buffers for all of the input data
    std::vector<cl::Buffer> buffers;
    buffers.reserve(nWaves);
    for (size_t i = 0; i < nWaves; i+=1) {
        cl::Buffer buffer(context, CL_MEM_READ_WRITE, dataSizes.at(i), NULL, &status);
        if (status != CL_SUCCESS)
            throw int(NOMEM);
        buffers.push_back(buffer);
    }
    
    // and copy all of the data to the device
    for (size_t i = 0; i < nWaves; i+=1) {
        status = commandQueue.enqueueWriteBuffer(buffers.at(i), false, 0, dataSizes.at(i), dataPointers.at(i));
        if (status != CL_SUCCESS)
            throw int(NOMEM);
    }
    
    // set arguments for the kernel
    for (size_t i = 0; i < nWaves; i+=1) {
        status = kernel.setArg(i, buffers.at(i));
        if (status != CL_SUCCESS)
            throw int(NOMEM);
    }
    
    // perform the actual calculation
    status = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, globalRange, workgroupSize, NULL, NULL);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    // copy arguments back into the waves
    for (size_t i = 0; i < nWaves; i+=1) {
        status = commandQueue.enqueueReadBuffer(buffers.at(i), false, 0, dataSizes.at(i), dataPointers.at(i));
        if (status != CL_SUCCESS)
            throw int(NOMEM);
    }
    
    // block until everything is finished
    status = commandQueue.finish();
    if (status != CL_SUCCESS)
        throw int(NOMEM);
}

void VectorAdd(waveHndl waveA, waveHndl waveB, waveHndl waveC) {
    int err = 0;
    
    CountInt dimensionSizes[MAX_DIMENSIONS + 1];
    int numDimensions;
    
    err = MDGetWaveDimensions(waveA, &numDimensions, dimensionSizes);
    if (err)
        throw int(err);
    CountInt nPoints = dimensionSizes[0];
    
    err = MDGetWaveDimensions(waveB, &numDimensions, dimensionSizes);
    if (err)
        throw int(err);
    if (dimensionSizes[0] != nPoints)
        throw int(NOMEM);
    
    err = MDGetWaveDimensions(waveB, &numDimensions, dimensionSizes);
    if (err)
        throw int(err);
    if (dimensionSizes[0] != nPoints)
        throw int(NOMEM);
    
    
    float *arrayA = reinterpret_cast<float*>(WaveData(waveA));
    float *arrayB = reinterpret_cast<float*>(WaveData(waveB));
    float *arrayC = reinterpret_cast<float*>(WaveData(waveC));
    
    // get the first available platform
    cl_int status;
    std::vector<cl::Platform> platforms;
    status = cl::Platform::get(&platforms);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    // get device to use
    std::vector<cl::Device> devices;
    status = platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    if (devices.empty())
        throw int(NOMEM);
    
    cl::Device device = devices[0];
    
    // context
    cl::Context context(device, NULL, NULL, NULL, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    // queue
    cl::CommandQueue commandQueue(context, device, 0, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    // fetch the contents of the program
    std::ifstream fileIn("/Users/peter/Documents/C++/XOP Toolkit 6/IgorXOPs6/IgorCL/Kernels.cl", std::ios::in | std::ios::ate);
    size_t fileSize = fileIn.tellg();
    fileIn.seekg(0);
    std::string programSource;
    programSource.resize(fileSize);
    fileIn.read(&programSource[0], fileSize);
    
    // now compile the source code
    cl::Program program(context, programSource, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    std::vector<cl::Device> theDeviceVector;
    theDeviceVector.push_back(device);
    status = program.build(theDeviceVector);
    if (status != CL_SUCCESS) {
        std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);

        //status = program.getBuildInfo<std::string>(device, CL_PROGRAM_BUILD_LOG, &buildLog);
        for (int i = 0; i < buildLog.size(); ++i) {
            if (buildLog[i] == '\n')
                buildLog[i] = '\r';
        }
        XOPNotice(buildLog.c_str());
        throw int(NOMEM);
    }
    
    cl::Kernel kernel(program, "VectorAdd", &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    // create buffers for input and output data
    int sizeInBytes = nPoints * sizeof(arrayA[0]);
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY, sizeInBytes, NULL, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeInBytes, NULL, &status);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    status = commandQueue.enqueueWriteBuffer(bufferA, false, 0, sizeInBytes, arrayA);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    status = commandQueue.enqueueWriteBuffer(bufferB, false, 0, sizeInBytes, arrayB);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);
    
    cl::NDRange workSpace(nPoints);
    status = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, workSpace);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    status = commandQueue.enqueueReadBuffer(bufferC, false, 0, sizeInBytes, arrayC);
    if (status != CL_SUCCESS)
        throw int(NOMEM);
    
    status = commandQueue.finish();
    if (status != CL_SUCCESS)
        throw int(NOMEM);
}