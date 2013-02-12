//
//  IgorCLOperations.h
//  IgorCL
//
//  Created by Peter Dedecker on 05/02/2013.
//
//

#ifndef IgorCL_IgorCLOperations_h
#define IgorCL_IgorCLOperations_h

#include "XOPStandardHeaders.h"
#include <vector>
#include "cl.hpp"

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::string& sourceText);
void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<int>& memFlags, const std::vector<char>& sourceBinary);

std::vector<char> compileSource(const int platformIndex, const int deviceIndex, const std::string programSource);

#endif
