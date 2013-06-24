//
//  IgorCLUtilities.h
//  IgorCL
//
//  Created by Peter Dedecker on 06/02/2013.
//
//

#ifndef __IgorCL__IgorCLUtilities__
#define __IgorCL__IgorCLUtilities__

#include <string>
#include <vector>
#include <mutex>
#include <boost/smart_ptr.hpp>

#include "XOPStandardHeaders.h"
#include "cl.hpp"

void StoreStringInTextWave(const std::string str, waveHndl textWave, IndexInt* indices);
std::string GetStdStringFromHandle(const Handle handle);
Handle PutStdStringInHandle(const std::string theString);

size_t WaveDataSizeInBytes(waveHndl wave);
size_t SharedMemorySizeFromWave(waveHndl wave);

int GetFirstDeviceOfType(const int platformIndex, const std::string& deviceTypeStr);
int ConvertIgorCLFlagsToOpenCLFlags(const int igorCLFlags);

class IgorCLContextAndDeviceProvider {
public:
    IgorCLContextAndDeviceProvider() {;}
    ~IgorCLContextAndDeviceProvider() {;}
    
    void getContextForPlatformAndDevice(const int platformIndex, const int deviceIndex, cl::Context& context, cl::Device &device);
    
private:
    std::vector<std::pair<int, int> > availableContextIndices;
    std::vector<cl::Context> availableContexts;
    std::vector<cl::Device> deviceForContext;
    
    std::mutex contextMutex;
};

extern IgorCLContextAndDeviceProvider contextAndDeviceProvider;


#endif /* defined(__IgorCL__IgorCLUtilities__) */
