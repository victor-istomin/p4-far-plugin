#include "stdafx.h"
#include "FarMenu.h"
#include "FarGlobal.h"

const FarKey FarMenu::s_nullKey = {0};

FarMenu::Item::Item( const Command& handler, intptr_t messageId, intptr_t userData /*= NULL*/, 
                     const FarKey& accelerator /*= s_nullKey*/, MENUITEMFLAGS flags /*= MIF_NONE*/ )
    : m_handler    (handler)
    , m_message    (messageId)
    , m_userData   (userData)
    , m_accelerator(accelerator)
    , m_flags      (flags)
{
}

FarMenuItem FarMenu::Item::get() const
{
    FarMenuItem item = {0};
    item.AccelKey = m_accelerator;
    item.Flags    = m_flags;
    item.Text     = FarGlobal::GetMsg(m_message);
    return item;
}

FarMenu::~FarMenu()
{
}

intptr_t FarMenu::run() const
{
    std::vector<FarMenuItem> items;
    std::copy(begin(m_items), end(m_items), std::back_inserter(items));
    const wchar_t* title  = m_titleId  != -1  ?  FarGlobal::GetMsg(m_titleId)   :  NULL;
    const wchar_t* bottom = m_bottomId != -1  ?  FarGlobal::GetMsg(m_bottomId)  :  NULL;

    std::vector<FarKey> breakKeys;
    std::for_each(begin(m_breakKeys), end(m_breakKeys), [&breakKeys](const FarMenu::Break& b){breakKeys.push_back(b.key());});
    if(!breakKeys.empty())
        breakKeys.push_back(s_nullKey);   // terminating null

    const FarKey* breakKeysPtr  = breakKeys.empty() ? NULL : &breakKeys[0];
    intptr_t      breakKeyIndex = -1;

    intptr_t selected = FarGlobal::Menu(m_menu, -1, -1, 0, m_flags, title, bottom, m_help, breakKeysPtr, &breakKeyIndex, &items[0], items.size());

    if ( selected >= 0 && selected < static_cast<intptr_t>(items.size()) ) // is a menu command or shortcut
    {
        selected = (breakKeyIndex != -1)  ?  m_breakKeys[breakKeyIndex].execute()  :  m_items[selected].execute();
    }

    return selected;
}
