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

void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::string& sourceText);
void DoOpenCLCalculation(const int platformIndex, const int deviceIndex, const cl::NDRange globalRange, const cl::NDRange workgroupSize, const std::string& kernelName, const std::vector<waveHndl>& waves, const std::vector<char>& sourceBinary);

void VectorAdd(waveHndl waveA, waveHndl waveB, waveHndl waveC);

#endif
