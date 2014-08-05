/* Copyright (C) Sven Erik Knop, Perforce Software, 2009
 *
*/
#include "stdafx.h"
#include <windows.h>
#include <ctime>
#include <stdexcept>

#include "PerforceClient.h"
#include "FarGlobal.h"
#include "messages.h"
#include "stringUtils.h"

namespace 
{
    class OutputClient : public ClientUser
    {
    public:
        OutputClient() : m_isError(false), m_isExpired(false) {} 

        virtual void OutputInfo( char level, const char *data ) 
        {
            m_buf << data << "\n";
        }

        virtual void OutputError(const char* errBuf) 
        {
            m_isError = true;
            m_err << errBuf;
            if (strstr(errBuf, "expired") && strstr(errBuf, "login again"))
                m_isExpired = true;
        }

        virtual void OutputStat(StrDict* varList) 
        {
            StrRef variable, data;

            for (int i = 0; varList->GetVar(i, variable, data); i++ ) 
            {
                if (variable == "func" || variable == P4Tag::v_specFormatted) 
                    continue;

                m_buf << variable << " ... " << data << "\n";
                m_dict.VSetVar(variable, data);
            }
        }

        bool isErrorExists() const      { return m_isError; }
        bool isTicketExpired() const    { return m_isExpired; }
    
        const StrBuf& getError() const  { return m_err; }
        const StrBuf& getBuf() const    { return m_buf; }
        const StrDict& getDict() const  { return m_dict; }

    private:
        bool        m_isExpired;
        bool        m_isError;
        StrBuf      m_err;
        StrBuf      m_buf;
        StrBufDict  m_dict;
    };
}

PerforceClient::PerforceClient(const GUID* messagesId)
    : m_messagesId(messagesId)
{
    std::string currentDir;
    FarGlobal::GetCurrentDirectory(currentDir);

    m_error.reset(new Error);
    m_client.reset(new ClientApi);
	m_client->SetCwd(currentDir.c_str());
    m_client->Init(m_error.get());

    if (m_error->Test()) 
		exception("Connection failed", m_error.get());
}

PerforceClient::~PerforceClient()
{
    m_client->Final(m_error.get());
    m_client.reset();
}

void PerforceClient::exception(const char *msg, Error *e)
{
	StrBuf error;
	e->Fmt(&error);

	StrBuf message;
	message << msg << ": " << error;
	throw std::runtime_error(message.Text());
}

bool PerforceClient::displayMessage(int titleId, const StrRef &buf, bool allowCancel)
{
    FARMESSAGEFLAGS type = allowCancel ? FMSG_MB_OKCANCEL : FMSG_MB_OK;

    int result = FarGlobal::Message(m_messagesId, type |  FMSG_LEFTALIGN, titleId, ansi2wide(buf.Text()).c_str());
    return (result == 0/*OK button*/);
}

// ----- Perforce services -----

void PerforceClient::showInfo()
{
	OutputClient cu;
	m_client->Run("info", &cu);
	displayMessage(MP4ClientInformationTitle, cu.getBuf());
}

// TODO - implement
void PerforceClient::callP4V(const std::wstring& command, const std::wstring& file)
{
    std::wstring path;
    FarGlobal::GetCurrentDirectoryW(path);

    std::wstring cmdLine = L"p4v.exe";
    cmdLine += ansi2wide(" -p " + std::string(m_client->GetPort().Text()));
    cmdLine += ansi2wide(" -c " + std::string(m_client->GetClient().Text()));
    cmdLine += ansi2wide(" -u " + std::string(m_client->GetUser().Text()));
    
	if( m_client->GetCharset().Length() )
		cmdLine += ansi2wide(" -C " + std::string(m_client->GetCharset().Text()));

    if ( m_client->GetPassword().Length() )
		cmdLine += ansi2wide(" -P " + std::string(m_client->GetPassword().Text()));

    if ( !command.empty() || !file.empty())
	    cmdLine += L" -cmd \"" + command + (file.empty() ? L"" : L" ") + file + L"\"";

	// OK, here we actually are going to call P4V.

	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;

	::GetStartupInfo( &startInfo );
	startInfo.lpReserved = startInfo.lpDesktop = NULL; 
	startInfo.dwFlags |= STARTF_USESHOWWINDOW;
	startInfo.wShowWindow = SW_SHOWNORMAL;

	::CreateProcess( NULL, &cmdLine[0],
				   NULL, NULL,
				   FALSE, NORMAL_PRIORITY_CLASS,//inheritance & creation flags 
				   NULL, NULL,  				//default env and current dir
				   &startInfo, &procInfo );     

	::CloseHandle( procInfo.hThread );  
}
/*
void PerforceClient::showFileInfo()
{
	TCHAR path[MAX_PATH];
	
	::SendMessage(nppData._nppHandle, NPPM_GETFULLCURRENTPATH, 0, (LPARAM)path);
	char file[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, path, -1, file, MAX_PATH, NULL, NULL);

	int argc = 2;
	char * argv[] = { "-l", file };

	client.SetArgv( argc, argv );

	OutputClient ui;

	// client.SetVar( P4Tag::v_tag );
	client.Run("fstat", &ui);

	if( ui.ticketExpired() ) {
		displayMessage(TEXT("Ticket expired"), ui.getError());
		return;
	}
	if( ui.errorExists() ) {
		displayMessage(TEXT("Perforce error"), ui.getError());
		return;
	}

	StrBuf buf;
	StrDict &dict = ui.getDict();

	if( dict.GetVar("depotFile") ) {
		buf << "Depot File :\t" << dict.GetVar("depotFile") << "\n";
		buf << "Client File :\t" << dict.GetVar("clientFile") << "\n";
		if( dict.GetVar("haveRev") && dict.GetVar("headRev") ) {
			buf << "Revision : \t\t#" << dict.GetVar("haveRev") << " of " << dict.GetVar("headRev") << "\n";
		}
		else {
			buf << "Revision : \t\t#0/0\n";
		}

		buf << "Change : \t\t" << dict.GetVar("headChange") << "\n";

		const size_t MAX_DATE = 128;
		char dateBuffer[MAX_DATE];
		time_t rawtime = dict.GetVar("headTime")->Atoi();

		struct tm timeinfo;
		localtime_s(&timeinfo, &rawtime);

		strftime(dateBuffer, MAX_DATE, "%#c", &timeinfo);

		buf << "Last changed : \t" << dateBuffer << "\n";

		buf << "Type : \t\t" << dict.GetVar("headType") << "\n";

		if( dict.GetVar("fileSize") ) {
			buf << "File size : \t\t" << dict.GetVar("fileSize") << "\n";
		}

		StrBuf openBy("Opened by ");

		if( dict.GetVar("action") ) {
			buf << "\nOpen for " << dict.GetVar("action") << " - change " << dict.GetVar("change") << " (" << dict.GetVar("type") << ")\n";
			openBy = "\tAlso opened by ";
		}

		if( dict.GetVar("otherOpen") ) {
			int otherOpen = atoi(dict.GetVar("otherOpen")->Text());

			buf << "\n";

			for( int i = 0; i < otherOpen; i++) {
				StrBuf name("otherOpen");
				StrBuf action("otherAction");

				name << i;
				action << i;
				buf << openBy << dict.GetVar(name) << " for " << dict.GetVar(action) << "\n";
			}
		}
	}
	else {
		buf << "Not stored in Perforce";
	}

	displayMessage(TEXT("Perforce file info"), buf /*or ui.getBuf()?*//*);
}
*/
void PerforceClient::processFiles(const char *cmd,  const TFileNames& files)
{
    if(files.empty())
        return;

	std::vector<char*> argv;
    TFileNames argvFiles = files;
    std::for_each(begin(argvFiles), end(argvFiles), [&argv](std::string& s){argv.push_back(&s[0]);});
	
	m_client->SetArgv(argv.size(), &argv[0]);

	OutputClient ui;
	m_client->Run( cmd, &ui );

	if(ui.isTicketExpired() || ui.isErrorExists()) 
    {
		displayMessage(MTitle, ui.getError());
	}

    // todo - display  warnings like editing remote depot, etc.
}