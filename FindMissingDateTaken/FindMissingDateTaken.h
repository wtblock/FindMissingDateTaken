/////////////////////////////////////////////////////////////////////////////
// Copyright © by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "resource.h"
#include <vector>
#include <map>
#include <memory>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// used for gdiplus library
ULONG_PTR m_gdiplusToken;

/////////////////////////////////////////////////////////////////////////////
// parse the extension from a pathname
static inline CString GetExtension( LPCTSTR pcszPath )
{
	CString csPath( pcszPath );
	TCHAR* pBuf = csPath.GetBuffer( csPath.GetLength() + 1 );
	TCHAR szDrive[ _MAX_DRIVE ];
	TCHAR szDir[ _MAX_DIR ];
	TCHAR szFile[ _MAX_FNAME ];
	TCHAR szExt[ _MAX_EXT ];

	_tsplitpath( pBuf, szDrive, szDir, szFile, szExt );
	csPath.ReleaseBuffer();
	return szExt;
}

/////////////////////////////////////////////////////////////////////////////
// initialize GDI+
bool InitGdiplus()
{
	GdiplusStartupInput gdiplusStartupInput;
	Status status = GdiplusStartup
	(
		&m_gdiplusToken,
		&gdiplusStartupInput,
		NULL
	);

	return ( Ok == status );
} // InitGdiplus

/////////////////////////////////////////////////////////////////////////////
// remove reference to GDI+
void TerminateGdiplus()
{
	GdiplusShutdown( m_gdiplusToken );
	m_gdiplusToken = NULL;

}// TerminateGdiplus

/////////////////////////////////////////////////////////////////////////////
