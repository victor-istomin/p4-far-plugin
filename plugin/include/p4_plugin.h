// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the P4_PLUGIN_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// P4_PLUGIN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef P4_PLUGIN_EXPORTS
#define P4_PLUGIN_API __declspec(dllexport)
#else
#define P4_PLUGIN_API __declspec(dllimport)
#endif

// This class is exported from the p4_plugin.dll
class P4_PLUGIN_API Cp4_plugin {
public:
	Cp4_plugin(void);
	// TODO: add your methods here.
};

extern P4_PLUGIN_API int np4_plugin;

P4_PLUGIN_API int fnp4_plugin(void);
