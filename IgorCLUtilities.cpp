//
//  IgorCLUtilities.cpp
//  IgorCL
//
//  Created by Peter Dedecker on 06/02/2013.
//
//

#include "IgorCLUtilities.h"

void StoreStringInTextWave(std::string str, waveHndl textWave, IndexInt* indices) {
    int strLength = str.size();
    
    Handle textHandle = NewHandle(strLength);
    if (textHandle == NULL)
        throw std::bad_alloc();
    
    int err = PutCStringInHandle(str.c_str(), textHandle);
    if (err)
        throw int(err);
    
    err = MDSetTextWavePointValue(textWave, indices, textHandle);
    if (err)
        throw int(err);
    
    DisposeHandle(textHandle);
}