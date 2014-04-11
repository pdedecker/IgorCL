//
//  IgorCLOperations.h
//  IgorCL
//
//  Created by Peter Dedecker on 05/02/2013.
//
//

#ifndef IgorCL_IgorCLOperations_h
#define IgorCL_IgorCLOperations_h

#include "cl.hpp"
#include "XOPStandardHeaders.h"
#include <vector>

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::string& sourceText);
void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::vector<char>& sourceBinary);

std::vector<char> CompileSource(const int platformIndex, const int deviceIndex, const std::string programSource, std::string& buildLog);

#endif
