#pragma once
#include <plugin.hpp>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

/**
 * Far menu wrapper
 */
class FarMenu
{
public:
    static const FarKey s_nullKey;
    typedef std::function< intptr_t(intptr_t/*userData*/) > TCommandFunction;

    //! provide simple interface for menu item handlers and sub-menus
    class Command
    {
        TCommandFunction m_function;
        static intptr_t submenuWrapper(const FarMenu& submenu, intptr_t /*not used*/)
        {
            return submenu.run();
        }

    public:
        Command(const TCommandFunction& function) : m_function(function)                                      {}
        Command(const FarMenu& submenu)           : m_function( [&submenu](intptr_t){return submenu.run();} ) {}

        intptr_t execute(intptr_t userData) const {return m_function(userData);}
    };


    //! used to break menu by hotkey
    class Break
    {
        FarKey   m_key;
        Command  m_command;
        intptr_t m_userData;

    public:
        Break(const FarKey& key, const Command& command, intptr_t userData) 
            : m_key(key), m_command(command), m_userData(userData) {}

        const FarKey& key()     const { return m_key; }
        intptr_t      execute() const { return m_command.execute(m_userData); }
    };

    //! menu item wrapper. Will re-read correct language strings on each get()
    class Item
    {
    public:
        Item(const Command& handler, intptr_t messageId, intptr_t userData = NULL, 
             const FarKey& accelerator = s_nullKey, MENUITEMFLAGS flags = MIF_NONE);

        FarMenuItem get() const;
        operator FarMenuItem() const { return get(); }

        intptr_t execute() const { return m_handler.execute(m_userData); }

    private:
        Command       m_handler;
        intptr_t      m_message;
        intptr_t      m_userData;
        FarKey        m_accelerator;
        MENUITEMFLAGS m_flags;
    };

    //! type of menu items storage
    typedef std::vector<Item>  MenuItems;

    //! type of break keys and their break codes
    typedef std::vector<Break> BreakKeys;

    FarMenu(const GUID& menu, const MenuItems& items, const wchar_t* help = NULL, FARMENUFLAGS flags = FMENU_AUTOHIGHLIGHT|FMENU_WRAPMODE, 
            intptr_t titleId = -1, intptr_t bottomId = -1, const BreakKeys& breaks = BreakKeys())
        : m_menu       (menu)
        , m_items      (items)
        , m_help       (help)
        , m_flags      (flags)
        , m_titleId    (titleId)
        , m_bottomId   (bottomId)
        , m_breakKeys  (breaks)
    {
    }

    ~FarMenu();

    intptr_t run() const;

private:
    const GUID&    m_menu;
    MenuItems      m_items;
    const wchar_t* m_help;
    FARMENUFLAGS   m_flags;
    intptr_t       m_titleId;     //! title message ID, or -1 if not needed
    intptr_t       m_bottomId;    //! bottom message ID, or -1 if not needed
    BreakKeys      m_breakKeys;   //! additional keys to break dialog (like Ins, F4 in user menu)
};

//! shortcut to cast c-array[] to vector
template <size_t N> FarMenu::BreakKeys makeBreakKeys(const FarMenu::Break (&items)[N])
{
    return FarMenu::BreakKeys(std::begin(items), std::end(items));
}

//! shortcut to cast c-array[] to vector
template <size_t N> FarMenu::MenuItems makeMenuItems(const FarMenu::Item (&items)[N])
{
    return FarMenu::MenuItems(std::begin(items), std::end(items));
}

//! shortcut to construct FarMenu::Break
template <class Callable>
FarMenu::Break makeBreakItem(WORD keyCode, DWORD keyState, const Callable& command, intptr_t userData = 0)
{
    FarKey key = {0};
    key.VirtualKeyCode  = keyCode;
    key.ControlKeyState = keyState;

    return FarMenu::Break(key, FarMenu::Command(command), userData);
}
