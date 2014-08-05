#include <stdafx.h>
#include "FarGlobal.h"
#include "stringUtils.h"
#include "initStructSize.hpp"

const wchar_t* FarGlobal::GetMsg( int messageId )
{
    const FarGlobal* i = instance();
    return i->m_api.GetMsg(i->m_mainGuid, messageId);
}

int FarGlobal::Menu( const GUID& id, intptr_t x, intptr_t y, intptr_t maxHeight, FARMENUFLAGS flags, const wchar_t* title, const wchar_t* bottom, const wchar_t* helpTopic, const FarKey* breakKeys, intptr_t* breakCode, const FarMenuItem* item, size_t itemsNumber )
{
    const FarGlobal* i = instance();
    return i->m_api.Menu(i->m_mainGuid, &id, x, y, maxHeight, flags, title, bottom, helpTopic, breakKeys, breakCode, item, itemsNumber);
}

FarGlobal* FarGlobal::instance( const PluginStartupInfo& api /*= PluginStartupInfo()*/, const GUID* mainGuid /*= nullptr*/ )
{
    static std::unique_ptr<FarGlobal> instance;
    if ( mainGuid != nullptr )
    {
        // initialization
        assert(!instance.get() && "Double initialization!");
        instance.reset(new FarGlobal(api, mainGuid));
    }

    assert(instance.get() && "Make sure singleton in properly initialized before use");
    return instance.get();
}

intptr_t FarGlobal::EditorControl( intptr_t EditorID, EDITOR_CONTROL_COMMANDS Command, intptr_t Param1, void *Param2 )
{
    const FarGlobal* i = instance();
    return i->m_api.EditorControl(EditorID, Command, Param1, Param2);
}

void FarGlobal::GetCurrentDirectory(std::wstring& path)
{
    const FarGlobal* i = instance();

    size_t pathSize = i->m_api.FSF->GetCurrentDirectoryW(0, nullptr);
    path.resize(pathSize);
    i->m_api.FSF->GetCurrentDirectoryW(pathSize, &path[0]);
}

void FarGlobal::GetCurrentDirectory(std::string& path)
{
    std::wstring wide;
    FarGlobal::GetCurrentDirectory(wide);
    
    path = wide2ansi(wide);
}

intptr_t FarGlobal::Message(const GUID* id, FARMESSAGEFLAGS flags, int captionMessageId, const wchar_t* message, const wchar_t* helpTopic)
{
    const FarGlobal* i = instance();
    std::wstring farMessage = FarGlobal::GetMsg(captionMessageId);
    farMessage += L"\n";
    farMessage += message;

    if ( std::count(begin(farMessage), end(farMessage), L'\n')  < 2 )
        farMessage += L"\n";
    
    return i->m_api.Message(i->m_mainGuid, id, flags | FMSG_ALLINONE, helpTopic, reinterpret_cast<const wchar_t* const*>(farMessage.c_str()), 0, 0);
}

FarGlobal::TPanelItems FarGlobal::GetSelectedItems(TPanelDirectoryPtr& directoryPtr)
{
    TPanelItems items;
    PanelInfo panelInfo;
    initStructSize(&panelInfo);
        
    FarGlobal* i = instance();

    const size_t directoryDataSize = i->m_api.PanelControl(PANEL_ACTIVE, FCTL_GETPANELDIRECTORY, 0, nullptr);
    directoryPtr.reset(directoryDataSize);
    initStructSize(directoryPtr.get());
    i->m_api.PanelControl(PANEL_ACTIVE, FCTL_GETPANELDIRECTORY, directoryPtr.size(), directoryPtr.get());

    i->m_api.PanelControl(PANEL_ACTIVE, FCTL_GETPANELINFO, NULL, &panelInfo);
    for(size_t number=0; number < panelInfo.SelectedItemsNumber; ++number)
    {
        const size_t dataSize = i->m_api.PanelControl(PANEL_ACTIVE, FCTL_GETSELECTEDPANELITEM, number, 0);  // is different for different items!
        TPanelItemPtr buffer(dataSize);
        std::fill(std::begin(buffer), std::end(buffer), 0);

        FarGetPluginPanelItem getItem;
        getItem.Item = buffer.get();
        getItem.Size = buffer.size();
        getItem.StructSize = sizeof(getItem);

        i->m_api.PanelControl(PANEL_ACTIVE, FCTL_GETSELECTEDPANELITEM, number, &getItem);

        items.emplace(items.end(), std::move(buffer));
    }

    return items;
}

