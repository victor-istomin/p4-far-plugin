#include "stdafx.h"
#include <plugin.hpp>
#include <DlgBuilder.hpp>

#include "LoginDialog.h"
#include "FarGlobal.h"
#include "messages.h"
#include "guid.hpp"

bool CLoginDialog::Run()
{
    FarGlobal* i = FarGlobal::instance();
    PluginDialogBuilder builder(i->api(), i->mainGuid(), DlgLoginGuid, MLoginTitle, nullptr);

    const size_t bufferSize = max(max(max(1024, m_credentials.workspace.size()), m_credentials.login.size()), m_credentials.password.size());
    std::wstring loginBuffer    (bufferSize, L'\0');
    std::wstring workspaceBuffer(bufferSize, L'\0');
    std::wstring passwordBuffer (bufferSize, L'\0');
    BOOL         remember = m_credentials.savePassword ? TRUE : FALSE;

    std::copy(begin(m_credentials.login),     end(m_credentials.login),     begin(loginBuffer));
    std::copy(begin(m_credentials.workspace), end(m_credentials.workspace), begin(workspaceBuffer));
    std::copy(begin(m_credentials.password),  end(m_credentials.password),  begin(passwordBuffer));

    const size_t width = 30;
    auto loginEdit = builder.AddEditField(&loginBuffer[0], loginBuffer.size(), width);   // TODO: history
    builder.AddTextBefore(loginEdit, MP4LoginUsernameMessage);

    auto workspaceEdit = builder.AddEditField(&workspaceBuffer[0], workspaceBuffer.size(), width); // TODO: history
    builder.AddTextBefore(workspaceEdit, MP4LoginWorkspaceMessage);

    auto passwordEdit = builder.AddPasswordField(&passwordBuffer[0], passwordBuffer.size(), width);
    builder.AddTextBefore(passwordEdit, MP4LoginPasswordMessage);

    if (m_enablePasswordCheckbox)
    {
        builder.AddSeparator();
        builder.AddCheckbox(MP4LoginRememberPassword, &remember);
    }

    builder.AddOKCancel(MP4LoginOkButton, MP4LoginCancelButton);
    bool isOk = builder.ShowDialog();

    if (isOk)
    {
        m_credentials = Credentials(loginBuffer, workspaceBuffer, passwordBuffer, remember != FALSE);
    }

    return isOk;
}
