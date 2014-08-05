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

    //! used to break menu by hotkey
    struct Break
    {
        FarKey   m_key;
        intptr_t m_code;

        operator FarKey() const   { return m_key; }
        operator intptr_t() const { return m_code; }
    };

    //! menu item wrapper. Will re-read correct language strings on each get()
    class Item
    {
    public:
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

    FarMenu(const GUID& menu, const MenuItems& items, const wchar_t* help = NULL, FARMENUFLAGS flags = FMENU_AUTOHIGHLIGHT, 
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

    template <size_t N> 
    FarMenu(const GUID& menu, const Item (&items)[N], const wchar_t* help = NULL, FARMENUFLAGS flags = FMENU_AUTOHIGHLIGHT, 
            intptr_t titleId = -1, intptr_t bottomId = -1, const BreakKeys& breaks = BreakKeys())
        : m_menu       (menu)
        , m_items      ()
        , m_help       (help)
        , m_flags      (flags)
        , m_titleId    (titleId)
        , m_bottomId   (bottomId)
        , m_breakKeys  (breaks)
    {
        std::copy(std::begin(items), std::end(items), std::back_inserter(m_items));
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

