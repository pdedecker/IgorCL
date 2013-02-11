//
//  IgorCLUtilities.cpp
//  IgorCL
//
//  Created by Peter Dedecker on 06/02/2013.
//
//

#include "IgorCLUtilities.h"

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