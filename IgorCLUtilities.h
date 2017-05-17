/*
IgorCL - an XOP to use OpenCL in Igor Pro
Copyright(C) 2013-2017 Peter Dedecker

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

The developer(s) of this software hereby grants permission to link
this program with Igor Pro, developed by WaveMetrics Inc. (www.wavemetrics.com).
*/

#ifndef __IgorCL__IgorCLUtilities__
#define __IgorCL__IgorCLUtilities__

#include "cl.hpp"
#include <string>
#include <vector>
#include <mutex>

#include "XOPStandardHeaders.h"

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

class IgorCLCommandQueueFactory {
public:
    IgorCLCommandQueueFactory() {;}
    ~IgorCLCommandQueueFactory() {;}
    
    cl::CommandQueue getCommandQueue(const int platformIndex, const int deviceIndex);
    void returnCommandQueue(const cl::CommandQueue commandQueue, const int platformIndex, const int deviceIndex);
    void deleteAllCommandQueues();
    
private:
    std::vector<std::pair<int, int> > _availableQueueIndices;
    std::vector<std::vector<cl::CommandQueue> > _availableQueues;
    
    std::mutex _queueMutex;
};

extern IgorCLCommandQueueFactory commandQueueFactory;

class IgorCLCommandQueueProvider {
public:
    IgorCLCommandQueueProvider(const int platformIndex, const int deviceIndex);
    ~IgorCLCommandQueueProvider();
    
    cl::CommandQueue getCommandQueue() const {return _commandQueue;}
    
private:
    int _platformIndex;
    int _deviceIndex;
    cl::CommandQueue _commandQueue;
};

std::string OpenCLErrorCodeToSymbolicName(int errorCode);


#endif /* defined(__IgorCL__IgorCLUtilities__) */
