#pragma once
#include <string>
#include <map>
struct ProcessEditorEventInfo;


/**
 * Editors session management. Keep track of files loaded in editor
 */
class FarEditor
{
public:
    //! editor session will be created on EE_READ event processing
    struct Session
    {
        intptr_t     m_id;
        std::wstring m_fileName;  

        explicit Session(intptr_t id = 0) : m_id(id), m_fileName()      {}

        intptr_t EditorControl(EDITOR_CONTROL_COMMANDS command, intptr_t param1, void* param2);
    };

    FarEditor();
    virtual ~FarEditor();

    intptr_t processEvent(const ProcessEditorEventInfo* info);

protected:
    virtual intptr_t onReadCallback(const Session& session) {return 0;}          //!< process new file read
    virtual intptr_t onCloseCallback(const Session& session){return 0;}          //!< process file close

private:
    typedef std::map<intptr_t/*ID*/, Session> TSessionsMap;

    TSessionsMap m_sessions;

    intptr_t onRead(const ProcessEditorEventInfo* info);                //!< create session and call callback
    intptr_t onClose(const ProcessEditorEventInfo* info);               //!< call callback  and delete session
};

