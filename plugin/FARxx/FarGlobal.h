#pragma once
#include <plugin.hpp>
#include <memory>
#include <cassert>
#include <string>
#include <vector>

#include "UniqueBuffer.h"

//! global information singleton. Not thread-safe initialization!
class FarGlobal
{
    PluginStartupInfo    m_api;
    FarStandardFunctions m_FSF;
    OpenInfo             m_openInfo;
    const GUID*          m_mainGuid;

    FarGlobal(const PluginStartupInfo& api, const GUID* mainGuid) 
        : m_api     (api)
        , m_FSF     (*api.FSF)
        , m_mainGuid(mainGuid)
    {
        m_api.FSF = &m_FSF;
        memset(&m_openInfo, 0, sizeof(m_openInfo));
    }

    FarGlobal(const FarGlobal&); //!< non-copyable
    FarGlobal& operator=(const FarGlobal&); //!< non-copyable

public:
    ~FarGlobal() {}

    static FarGlobal* instance(const PluginStartupInfo& api = PluginStartupInfo(), const GUID* mainGuid = nullptr);
    //static void updateOpenInfo(const OpenInfo* info);
    //static const OpenInfo& 

    //! Get language string
    static const wchar_t* GetMsg(int messageId);

    //! create a menu
    static int Menu(const GUID& id, intptr_t x, intptr_t y, intptr_t maxHeight, FARMENUFLAGS flags, 
        const wchar_t* title, const wchar_t* bottom, const wchar_t* helpTopic, const FarKey* breakKeys, intptr_t* breakCode,
        const FarMenuItem* Item, size_t itemsNumber);

    //! process editor command
    static intptr_t EditorControl(intptr_t EditorID, EDITOR_CONTROL_COMMANDS Command, intptr_t Param1, void *Param2);

    static void GetCurrentDirectory(std::wstring& path);        //!< get path to current directory in active pane
    static void GetCurrentDirectory(std::string& path);         //!< get path to current directory in active pane

    typedef UniqueBuffer<PluginPanelItem*>   TPanelItemPtr;
    typedef UniqueBuffer<FarPanelDirectory*> TPanelDirectoryPtr;
    typedef std::vector<TPanelItemPtr>       TPanelItems;

    static TPanelItems GetSelectedItems(TPanelDirectoryPtr& directoryPtr);

    //! display simple message. May contain '\n'
    static intptr_t Message(const GUID* id, FARMESSAGEFLAGS flags, int captionMessageId, const wchar_t* message, const wchar_t* helpTopic = NULL);
};

