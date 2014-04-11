//
//  IgorCLConstants.h
//  IgorCL
//
//  Created by Peter Dedecker on 12/02/2013.
//
//

#ifndef IgorCL_IgorCLConstants_h
#define IgorCL_IgorCLConstants_h

const int IgorCLReadWrite = 1 << 0;
const int IgorCLWriteOnly = 1 << 1;
const int IgorCLReadOnly = 1 << 2;
const int IgorCLUseHostPointer = 1 << 3;
const int IgorCLIsLocalMemory = 1 << 4;
const int IgorCLIsScalarArgument = 1 << 5;
const int IgorCLUsePinnedMemory = 1 << 6;

class IgorCLError {
public:
    IgorCLError(int e) {_errorCode = e;}
    ~IgorCLError() {;}
    
    int getErrorCode() const {return _errorCode;}
    
private:
    int _errorCode;
};

#endif
