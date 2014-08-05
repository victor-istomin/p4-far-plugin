#ifndef PERFORCE_CLIENT_H
#define PERFORCE_CLIENT_H
#include "NonCopyable.h"
#include <memory>
#include <string>
#include <vector>

// forward declarations
class ClientApi;
class Error;

/**
 * Perforce client wrapper
 */
class PerforceClient
{
public:
	explicit PerforceClient(const GUID* messagesId);
	~PerforceClient();

    typedef std::vector<std::string> TFileNames;

	void showInfo();
	//void showFileInfo();
	void addFiles   (const TFileNames& files)   { processFiles("add",    files); }
	void editFiles  (const TFileNames& files)   { processFiles("edit",   files); }
	void deleteFiles(const TFileNames& files)   { processFiles("delete", files); }
	void revertFiles(const TFileNames& files)   { processFiles("revert", files); }
	void syncFiles  (const TFileNames& files)   { processFiles("sync",   files); }
	
    void callP4V(const std::wstring& command = L"", const std::wstring& file = L"");

private:
	void exception(const char *msg, Error * e);
	void processFiles(const char* cmd, const TFileNames& files);
	bool displayMessage(int titleId, const StrRef& buf, bool allowCancel = false);

private:
	std::unique_ptr<ClientApi> m_client;
    std::unique_ptr<Error>     m_error;
    const GUID*                m_messagesId;
};

#endif
