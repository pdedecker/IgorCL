//
//  IgorCLConstants.h
//  IgorCL
//
//  Created by Peter Dedecker on 12/02/2013.
//
//

#ifndef IgorCL_IgorCLConstants_h
#define IgorCL_IgorCLConstants_h

#include "cl.hpp"

const int IgorCLReadWrite = 1 << 0;
const int IgorCLWriteOnly = 1 << 1;
const int IgorCLReadOnly = 1 << 2;
const int IgorCLUseHostPointer = 1 << 3;
const int IgorCLHostWriteOnly = 1 << 4;
const int IgorCLHostReadOnly = 1 << 5;
const int IgorCLHostNoAccess = 1 << 6;

#endif
