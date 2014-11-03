#pragma once
#include "NonCopyable.h"
#include <string>
#include <windows.h>

class CLoginDialog
{
public:
    struct Credentials
    {
        std::wstring login;
        std::wstring workspace;
        std::wstring password;
        bool         savePassword;

        Credentials(const std::wstring& l = L"", const std::wstring& w = L"", const std::wstring& p = L"", bool remember = false) 
            : login(l), workspace(w), password(p), savePassword(remember)   {}

        ~Credentials()
        {
            SecureZeroMemory(&login[0],     login.size());
            SecureZeroMemory(&workspace[0], workspace.size());
            SecureZeroMemory(&password[0],  password.size());
        }
    };

    ~CLoginDialog()                                                       {}
    CLoginDialog(const Credentials& defaults, bool enablePasswordCheckbox) 
        : m_credentials(defaults)
        , m_enablePasswordCheckbox(enablePasswordCheckbox)                {}

    const Credentials& credentials() const                                {return m_credentials;}
    bool Run();

private:
    Credentials m_credentials;
    bool        m_enablePasswordCheckbox;
};

