// p4_plugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "version.hpp"
#include "guid.hpp"
#include "p4_plugin.h"
#include "messages.h"

#include "initStructSize.hpp"
#include "stringUtils.h"
#include "FarMenu.h"
#include "FarGlobal.h"
#include "FarEditor.h"
#include "PerforceClient.h"

namespace
{
    std::string operator/(const std::string& left, const std::string& right)
    {
        return left.empty() || *left.rbegin() == '/' || *left.rbegin() == '\\' 
            ? left + right
            : left + '\\' + right;
    }
}

typedef void(PerforceClient::* TFilesFunction)(const PerforceClient::TFileNames& files);

FarEditor g_editor = FarEditor();

#if defined(__GNUC__)

#ifdef __cplusplus
extern "C"{
#endif
  BOOL WINAPI DllMainCRTStartup(HANDLE hDll,DWORD dwReason,LPVOID lpReserved);
#ifdef __cplusplus
};
#endif

BOOL WINAPI DllMainCRTStartup(HANDLE hDll,DWORD dwReason,LPVOID lpReserved)
{
	(void) lpReserved;
	(void) dwReason;
	(void) hDll;
	return TRUE;
}
#endif

void WINAPI GetGlobalInfoW(struct GlobalInfo* info)
{
	initStructSize(info);
	info->MinFarVersion = FARMANAGERVERSION;
	info->Version       = PLUGIN_VERSION;
	info->Guid          = MainGuid;
	info->Title         = PLUGIN_NAME;
	info->Description   = PLUGIN_DESC;
	info->Author        = PLUGIN_AUTHOR;
}

void WINAPI SetStartupInfoW(const PluginStartupInfo* psi)
{
	FarGlobal::instance(*psi, &MainGuid);
}

void WINAPI GetPluginInfoW(struct PluginInfo* info)
{
    static const wchar_t* PluginMenuStrings[] = { FarGlobal::GetMsg(MTitle) };

    initStructSize(info);
	info->Flags              = PF_EDITOR;
	info->PluginMenu.Guids   = &MenuGuid;
	info->PluginMenu.Strings = PluginMenuStrings;
	info->PluginMenu.Count   = ARRAYSIZE(PluginMenuStrings);
}

intptr_t WINAPI ProcessEditorEventW(const ProcessEditorEventInfo* editorInfo)
{
    return g_editor.processEvent(editorInfo);
}

intptr_t callP4V(intptr_t)
{
    try
    {
        PerforceClient client(&PerforceMessageGuid);
        client.callP4V();
    }
    catch (const std::exception& e)
    {
        ::MessageBoxA(0, e.what(), "Error", MB_ICONERROR);
    }
    return 0;
}

intptr_t displayGenericInformation(intptr_t)
{
    try
    {
        PerforceClient client(&PerforceMessageGuid);
        client.showInfo();
    }
    catch (const std::exception& e)
    {
        ::MessageBoxA(0, e.what(), "Error", MB_ICONERROR);
    }

    return 0;
}

intptr_t displayAboutInformation(intptr_t)
{
    ::MessageBoxA(NULL, "", "", 0);
    return 0;
}

intptr_t displaySettings(intptr_t)
{
    ::MessageBoxA(NULL, "", "", 0);
    return 0;
}

intptr_t p4_fileCommand(TFilesFunction function, intptr_t)
{
    FarGlobal::TPanelDirectoryPtr directoryPtr;
    const FarGlobal::TPanelItems selectedItems = FarGlobal::GetSelectedItems(directoryPtr);

    PerforceClient::TFileNames files;
    const std::string directory = wide2ansi(directoryPtr->Name);

    std::for_each(begin(selectedItems), end(selectedItems), [&files, &directory](const FarGlobal::TPanelItemPtr& item) 
    { 
        if (item->FileName[0] == L'\0')
            return;

        files.push_back(directory / wide2ansi(item->FileName));
    });

    try
    {
        PerforceClient client(&PerforceMessageGuid);
        (client.*function)(files);
    }
    catch (const std::exception& e)
    {
        ::MessageBoxA(0, e.what(), "Error", MB_ICONERROR);
    }
    
    return 0;
}

HANDLE WINAPI OpenW(const OpenInfo* openInfo)
{
    static const FarMenu::Item infoMenuItems[] = 
    { 
        FarMenu::Item(FarMenu::Item::Command(&displayGenericInformation), MMenuInformationGeneral), 
        FarMenu::Item(FarMenu::Item::Command(&displayAboutInformation),   MMenuInformationAbout)
    };

    static const FarMenu infoMenu = FarMenu(MenuInfoGuid, infoMenuItems);

    static const FarMenu::Item fileMenuItems[] = {
        FarMenu::Item(FarMenu::Item::Command(std::bind(&p4_fileCommand, &PerforceClient::editFiles,   std::placeholders::_1)), MMenuFileEdit),
        FarMenu::Item(FarMenu::Item::Command(std::bind(&p4_fileCommand, &PerforceClient::addFiles,    std::placeholders::_1)), MMenuFileAdd),
        FarMenu::Item(FarMenu::Item::Command(std::bind(&p4_fileCommand, &PerforceClient::deleteFiles, std::placeholders::_1)), MMenuFileDelete),
        FarMenu::Item(FarMenu::Item::Command(std::bind(&p4_fileCommand, &PerforceClient::revertFiles, std::placeholders::_1)), MMenuFileRevert),
        FarMenu::Item(FarMenu::Item::Command(std::bind(&p4_fileCommand, &PerforceClient::syncFiles,   std::placeholders::_1)), MMenuFileSync)
    };

    static const FarMenu fileMenu = FarMenu(MenuFileGuid, fileMenuItems);

    static const FarMenu::Item mainMenuItems[] = 
    {
        FarMenu::Item(infoMenu, MMenuMessageInfo), 
        FarMenu::Item(fileMenu, MMenuFileOperations),
        FarMenu::Item(FarMenu::Item::Command(&callP4V), MMenuCallPerforceGui),
        FarMenu::Item(FarMenu::Item::Command(&displaySettings), MMenuSettings)        
    };

    static const FarMenu menu = FarMenu(MenuGuid, mainMenuItems, nullptr, FMENU_AUTOHIGHLIGHT, MTitle);

    intptr_t result = menu.run();
    
	return nullptr;
}
