#include "XOPStandardHeaders.h"			// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h
#include "cl.hpp"

#include "IgorCL.h"
#include "IgorCLOperations.h"

// Global Variables (none)

// Runtime param structure for IGORCL operation.
#pragma pack(2)	// All structures passed to Igor are two-byte aligned.
struct IGORCLRuntimeParams {
	// Flag parameters.
    
	// Main parameters.
    
	// Parameters for simple main group #0.
	int waveAEncountered;
	waveHndl waveA;
	int waveAParamsSet[1];
    
	// Parameters for simple main group #1.
	int waveBEncountered;
	waveHndl waveB;
	int waveBParamsSet[1];
    
	// Parameters for simple main group #2.
	int waveCEncountered;
	waveHndl waveC;
	int waveCParamsSet[1];
    
	// These are postamble fields that Igor sets.
	int calledFromFunction;					// 1 if called from a user function, 0 otherwise.
	int calledFromMacro;					// 1 if called from a macro, 0 otherwise.
};
typedef struct IGORCLRuntimeParams IGORCLRuntimeParams;
typedef struct IGORCLRuntimeParams* IGORCLRuntimeParamsPtr;
#pragma pack()	// Reset structure alignment to default.

// Runtime param structure for IGORCLInfo operation.
#pragma pack(2)	// All structures passed to Igor are two-byte aligned.
struct IGORCLInfoRuntimeParams {
	// Flag parameters.
    
	// Main parameters.
    
	// These are postamble fields that Igor sets.
	int calledFromFunction;					// 1 if called from a user function, 0 otherwise.
	int calledFromMacro;					// 1 if called from a macro, 0 otherwise.
};
typedef struct IGORCLInfoRuntimeParams IGORCLInfoRuntimeParams;
typedef struct IGORCLInfoRuntimeParams* IGORCLInfoRuntimeParamsPtr;
#pragma pack()	// Reset structure alignment to default.

static int ExecuteIGORCL(IGORCLRuntimeParamsPtr p) {
	int err = 0;
    
	// Main parameters.
    waveHndl waveA, waveB, waveC;
	if (p->waveAEncountered) {
		// Parameter: p->waveA (test for NULL handle before using)
        if (p->waveA == NULL)
            return NOWAV;
        if (WaveType(p->waveA) != NT_FP32)
            return NOWAV;
        waveA = p->waveA;
	} else {
        return NOWAV;
    }
    
	if (p->waveBEncountered) {
		// Parameter: p->waveB (test for NULL handle before using)
        if (p->waveB == NULL)
            return NOWAV;
        if (WaveType(p->waveB) != NT_FP32)
            return NOWAV;
        waveB = p->waveB;
	} else {
        return NOWAV;
    }
    
	if (p->waveCEncountered) {
		// Parameter: p->waveC (test for NULL handle before using)
        if (p->waveC == NULL)
            return NOWAV;
        if (WaveType(p->waveC) != NT_FP32)
            return NOWAV;
        waveC = p->waveC;
	} else {
        return NOWAV;
    }
    
    try {
        VectorAdd(waveA, waveB, waveC);
    }
    catch (int e) {
        return e;
    }
    /*catch (cl::Error e) {
        XOPNotice(e.what());
        XOPNotice("\r");
        return GENERAL_BAD_VIBS;
    }*/
    catch (...) {
        return GENERAL_BAD_VIBS;
    }
    
	return err;
}

static int ExecuteIGORCLInfo(IGORCLInfoRuntimeParamsPtr p) {
	int err = 0;
    cl_int status;
    
    char noticeString[256];
    
    // fetch information on the platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    
    sprintf(noticeString, "Found %d platforms\r", platforms.size());
    XOPNotice(noticeString);
    
    std::string infoText;
    std::string deviceText;
    cl_bool clBool;
    cl_ulong clUlong;
    for (int i = 0; i < platforms.size(); ++i) {
        infoText = platforms[i].getInfo<CL_PLATFORM_VENDOR>();
        infoText += ", version ";
        infoText += platforms[i].getInfo<CL_PLATFORM_VERSION>();
        XOPNotice(infoText.c_str());
        XOPNotice("\r");
        
        std::vector<cl::Device> devices;
        status = platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
        deviceText = "Devices on this platform:\r";
        for (int j = 0; j < devices.size(); ++j) {
            cl_device_type deviceType = devices[j].getInfo<CL_DEVICE_TYPE>();
            if (deviceType == CL_DEVICE_TYPE_CPU) {
                deviceText += "CPU:\r";
            } else if (deviceType == CL_DEVICE_TYPE_GPU) {
                deviceText += "GPU:\r";
            } else if (deviceType == CL_DEVICE_TYPE_ACCELERATOR) {
                deviceText += "Accelerator:\r";
            } else {
                deviceText += "unknown:\r";
            }

			deviceText += "Device name:";
			deviceText += devices[j].getInfo<CL_DEVICE_NAME>();
			deviceText += "\r";
            
            deviceText += "OpenCL version:";
            deviceText += devices[j].getInfo<CL_DEVICE_VERSION>();
            deviceText += "\r";
            
            clBool = devices[j].getInfo<CL_DEVICE_AVAILABLE>();
            if (clBool) {
                deviceText += "device is available\r";
            } else {
                deviceText += "device is unavailable\r";
            }
            
            clUlong = devices[j].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
            sprintf(noticeString, "Global memory: %d\r", clUlong);
            deviceText += noticeString;
            clUlong = devices[j].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
            sprintf(noticeString, "Local memory: %d\r", clUlong);
            deviceText += noticeString;
            clUlong = devices[j].getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
            sprintf(noticeString, "Max allocation size: %d\r", clUlong);
            deviceText += noticeString;
            
            deviceText += "Device extensions: ";
            deviceText += devices[j].getInfo<CL_DEVICE_EXTENSIONS>();
            deviceText += "\r";
            
            XOPNotice(deviceText.c_str());
        }
    }
    
	return err;
}

static int RegisterIGORCL(void) {
	const char* cmdTemplate;
	const char* runtimeNumVarList;
	const char* runtimeStrVarList;
    
	// NOTE: If you change this template, you must change the IGORCLRuntimeParams structure as well.
	cmdTemplate = "IGORCL wave:waveA, wave:waveB, wave:waveC";
	runtimeNumVarList = "";
	runtimeStrVarList = "";
	return RegisterOperation(cmdTemplate, runtimeNumVarList, runtimeStrVarList, sizeof(IGORCLRuntimeParams), (void*)ExecuteIGORCL, 0);
}

static int RegisterIGORCLInfo(void) {
	const char* cmdTemplate;
	const char* runtimeNumVarList;
	const char* runtimeStrVarList;
    
	// NOTE: If you change this template, you must change the IGORCLInfoRuntimeParams structure as well.
	cmdTemplate = "IGORCLInfo ";
	runtimeNumVarList = "";
	runtimeStrVarList = "";
	return RegisterOperation(cmdTemplate, runtimeNumVarList, runtimeStrVarList, sizeof(IGORCLInfoRuntimeParams), (void*)ExecuteIGORCLInfo, 0);
}

static int
RegisterOperations(void) {
	int result;
	
	// Register IgorCL operation.
	if (result = RegisterIGORCL())
		return result;
    if (result = RegisterIGORCLInfo())
        return result;
	
	// There are no more operations added by this XOP.
		
	return 0;
}

/*	XOPEntry()

	This is the entry point from the host application to the XOP for all
	messages after the INIT message.
*/
extern "C" void
XOPEntry(void)
{	
	XOPIORecResult result = 0;
	
	switch (GetXOPMessage()) {
		// We don't need to handle any messages for this XOP.
	}
	SetXOPResult(result);
}

/*	main(ioRecHandle)

	This is the initial entry point at which the host application calls XOP.
	The message sent by the host must be INIT.
	
	main does any necessary initialization and then sets the XOPEntry field of the
	ioRecHandle to the address to be called for future messages.
*/
HOST_IMPORT int
XOPMain(IORecHandle ioRecHandle)
{
	int result;
	
	XOPInit(ioRecHandle);				// Do standard XOP initialization.
	SetXOPEntry(XOPEntry);				// Set entry point for future calls.
	
	if (igorVersion < 600) {			// Requires Igor Pro 6.00 or later.
		SetXOPResult(OLD_IGOR);			// OLD_IGOR is defined in IgorCL.h and there are corresponding error strings in IgorCL.r and IgorCLWinCustom.rc.
		return EXIT_FAILURE;
	}

	if (result = RegisterOperations()) {
		SetXOPResult(result);
		return EXIT_FAILURE;
	}
	
	SetXOPResult(0);
	return EXIT_SUCCESS;
}
