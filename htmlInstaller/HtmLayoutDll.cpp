#include "StdAfx.h"
#include "HtmLayoutDll.h"
#include <delayimp.h>

shared_any<HtmLayoutDll *, close_delete> HtmLayoutDll::Instance;

HtmLayoutDll::HtmLayoutDll()
	: m_h(NULL)
{
	Load();
}

HtmLayoutDll::~HtmLayoutDll()
{
	Unload();
}

void HtmLayoutDll::Unload()
{
	if (m_h != NULL)
	{
		::FreeLibrary(m_h);
		m_h = NULL;
	}

	__FUnloadDelayLoadedDLL2("htmlayout.dll");

	if (! m_path.empty())
	{
		// DVLib::FileDelete(m_path);
		::DeleteFile(m_path.c_str());
		m_path.clear();
	}

	if (! m_dir.empty())
	{
		::RemoveDirectory(m_dir.c_str());
		m_dir.clear();
	}
}

void HtmLayoutDll::Extract()
{
	std::wstring dir = L"htmlInstaller-";
	dir += DVLib::towstring(::GetCurrentProcessId());
	std::wstring path = DVLib::DirectoryCombine(DVLib::GetTemporaryDirectoryW(), dir);
	DVLib::DirectoryCreate(path);
	m_dir = path;
	path = DVLib::DirectoryCombine(path, L"htmlayout.dll");

	HRSRC res = ::FindResourceA(NULL, "HTMLAYOUTDLL", "DLL");
	CHECK_WIN32_BOOL(res != NULL,
		L"Error locating HTMLAYOUTDLL resource.");

	HGLOBAL hgl = ::LoadResource(NULL, res);
	CHECK_WIN32_BOOL(res != NULL,
		L"Error loading HTMLAYOUTDLL resource.");

	DWORD size = SizeofResource(NULL, res);
	CHECK_BOOL(size > 0,
		L"Invalid HTMLAYOUTDLL resource size.");

	LPVOID buffer = LockResource(hgl);
		
	auto_hfile h(CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, 
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));

	CHECK_WIN32_BOOL(get(h) != INVALID_HANDLE_VALUE,
		L"Error creating " << path << L".");

	DWORD written = 0;
	CHECK_WIN32_BOOL(WriteFile(get(h), buffer, size, & written, NULL),
		L"Error writing to " << path << L".");

	m_path = path;
}

void HtmLayoutDll::Load()
{
    HMODULE h = ::LoadLibraryA("htmlayout.dll");
	if (h == 0) 
	{
		Extract();
		h = ::LoadLibraryW(m_path.c_str());
	}
	
	CHECK_WIN32_BOOL(h != NULL,
		L"Error loading htmlayout.dll (" << m_path << L")");

	m_h = h;
}
