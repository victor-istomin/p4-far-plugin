#include "stdafx.h"
#include "FarEditor.h"
#include "FarGlobal.h"


intptr_t FarEditor::Session::EditorControl( EDITOR_CONTROL_COMMANDS command, intptr_t param1, void* param2 )
{
    return FarGlobal::EditorControl(m_id, command, param1, param2);
}

FarEditor::FarEditor()
{
}


FarEditor::~FarEditor()
{
}

intptr_t FarEditor::processEvent(const ProcessEditorEventInfo* info)
{
    intptr_t result = 0;
    if (info->StructSize < sizeof(*info))
        return result;     // old version
    
    switch (info->Event)
    {
    case EE_READ:
        result = onRead(info);
        break;

    case EE_CLOSE:
        result = onClose(info);
        break;

    case EE_SAVE:
    case EE_REDRAW:
    case EE_GOTFOCUS:
    case EE_KILLFOCUS:
    case EE_CHANGE:
        break; /// @todo
    }

    return result;
}

intptr_t FarEditor::onRead( const ProcessEditorEventInfo* info )
{
    Session& session = m_sessions[info->EditorID] = Session(info->EditorID);
    std::vector<wchar_t> buffer(session.EditorControl(ECTL_GETFILENAME, 0, nullptr), 0);
    session.EditorControl(ECTL_GETFILENAME, buffer.size(), &buffer[0]);
    session.m_fileName = &buffer[0];

    return onReadCallback(session);
}

intptr_t FarEditor::onClose( const ProcessEditorEventInfo* info )
{
    intptr_t result = onCloseCallback(m_sessions[info->EditorID]);
    m_sessions.erase(info->EditorID);
    return result;
}
