#pragma once
#include "fareditor.h"
#include <map>
#include <memory>

class P4FileEditor 
    : public FarEditor
{
    struct P4File
    {

    };

public:
    P4FileEditor();
    ~P4FileEditor();

    typedef decltype(Session::m_id) TId;
    typedef std::unique_ptr<P4File> TFilePtr;
    typedef std::map<TId, TFilePtr> TFiles;

protected:
    virtual intptr_t onReadCallback(const Session& session);
    virtual intptr_t onCloseCallback(const Session& session);

private:
    TFiles m_files;

};

