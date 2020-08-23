/////////////////////////////////////////////////////////////////////////////
// Copyright © 2020 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindMissingDateTaken.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;

/////////////////////////////////////////////////////////////////////////////
// returns true if the given ASCII date ID is found. Supported IDs are
// PropertyTagExifDTOrig and PropertyTagExifDTDigitized.
bool GetDateFound( Gdiplus::Image* pImage, PROPID id )
{
	bool value = false;

	// get the size of the date property
	const UINT uiSize = pImage->GetPropertyItemSize( id );

	// if the property exists, it will have a non-zero size 
	if ( uiSize > 0 )
	{
		// using a smart pointer which will release itself
		// when it goes out of context to allocate space
		// for the property item
		unique_ptr<Gdiplus::PropertyItem> pItem =
			unique_ptr<Gdiplus::PropertyItem>
			(
				(PropertyItem*)malloc( uiSize )
			);

		// Get the property item.
		pImage->GetPropertyItem( id, uiSize, pItem.get() );

		// the property should be ASCII
		if ( pItem->type == PropertyTagTypeASCII )
		{
			const CString csDate = (LPCSTR)pItem->value;
			value = true;
		}
	}

	return value;
} // GetDateFound

/////////////////////////////////////////////////////////////////////////////
// crawl through the directory tree looking for valid image extensions
void RecursePath( LPCTSTR path )
{
	USES_CONVERSION;

	// valid file extensions
	const CString csValidExt = _T( ".jpg;.jpeg;.png;.gif;.bmp;.tif;.tiff" );

	// the new folder under the image folder to contain the corrected images
	CString csPathname( path );
	csPathname.TrimRight( _T( "\\" ) );

	// build a string with wildcards
	CString strWildcard;
	strWildcard.Format( _T( "%s\\*.*" ), path );

	// start trolling for files we are interested in
	CFileFind finder;
	BOOL bWorking = finder.FindFile( strWildcard );
	while ( bWorking )
	{
		bWorking = finder.FindNextFile();

		// skip "." and ".." folder names
		if ( finder.IsDots() )
		{
			continue;
		}

		// if it's a directory, recursively search it
		if ( finder.IsDirectory() )
		{
			const CString str = finder.GetFilePath();

			// recurse into the new directory
			RecursePath( str );

		} else // read the properties if it is a valid extension
		{
			const CString csPath = finder.GetFilePath();
			const CString csExt = GetExtension( csPath ).MakeLower();

			if ( -1 != csValidExt.Find( csExt ) )
			{
				// smart pointer to the image representing this element
				unique_ptr<Gdiplus::Image> pImage =
					unique_ptr<Gdiplus::Image>
					(
						Gdiplus::Image::FromFile( T2CW( csPath ) )
					);

				// being pessimistic
				bool bOkay = false;

				// test for original date
				if ( GetDateFound( pImage.get(), PropertyTagExifDTOrig ))
				{
					bOkay = true;
				}

				// test for digitized date
				if ( GetDateFound( pImage.get(), PropertyTagExifDTDigitized ))
				{
					bOkay = true;
				}

				// if not okay, the date information is missing so record
				// the pathname to the output
				if ( bOkay == false )
				{
					CStdioFile fout( stdout );

					fout.WriteString( csPath + _T( "\n" ) );
				}
			}
		}
	}

	finder.Close();

} // RecursePath

/////////////////////////////////////////////////////////////////////////////
// a console application that can crawl through the file
// system and troll for image properties
int _tmain( int argc, TCHAR* argv[], TCHAR* envp[] )
{
	HMODULE hModule = ::GetModuleHandle( NULL );
	if ( hModule == NULL )
	{
		_tprintf( _T( "Fatal Error: GetModuleHandle failed\n" ) );
		return 1;
	}

	// initialize MFC and error on failure
	if ( !AfxWinInit( hModule, NULL, ::GetCommandLine(), 0 ) )
	{
		_tprintf( _T( "Fatal Error: MFC initialization failed\n " ) );
		return 2;
	}

	// we are expecting two parameters on the command line and if
	// this is not the case, present some usage information to
	// the user. NOTE: the first parameter is the executable path,
	// so we are only interested in the second parameter which
	// is the pathname of the folder to be processed.
	CStdioFile fOut( stdout );
	if ( argc != 2 )
	{
		fOut.WriteString( _T( ".\n" ) );
		fOut.WriteString
		(
			_T( "FindMissingDateTaken, Copyright (c) 2020, " )
			_T( "by W. T. Block.\n" )
		);

		fOut.WriteString
		( 
			_T( ".\n" ) 
			_T( "Usage:\n" )
			_T( ".\n" )
			_T( ".  FindMissingDateTaken pathname\n" )
			_T( ".\n" )
			_T( "Where:\n" )
			_T( ".\n" )
			_T( ".  pathname is the root of the tree to be scanned\n" )
			_T( ".\n" ) 
		);

		// return code three indicates incorrect number of parameters
		return 3;
	}

	// display the executable path (parameter zero)
	CString csMessage;
	csMessage.Format( _T( "Executable pathname: %s\n" ), argv[ 0 ] );
	fOut.WriteString( _T( ".\n" ) );
	fOut.WriteString( csMessage );
	fOut.WriteString( _T( ".\n" ) );

	// retrieve the pathname and validate the pathname exists
	// (parameter one)
	CString csPath = argv[ 1 ];
	if ( !::PathFileExists( csPath ) )
	{
		csMessage.Format( _T( "Invalid pathname: %s\n" ), csPath );
		fOut.WriteString( _T( ".\n" ) );
		fOut.WriteString( csMessage );
		fOut.WriteString( _T( ".\n" ) );

		// return code four indicates the pathname given does
		// not exist
		return 4;

	} else
	{
		csMessage.Format( _T( "Given pathname: %s\n" ), csPath );
		fOut.WriteString( _T( ".\n" ) );
		fOut.WriteString( csMessage );
		fOut.WriteString( _T( ".\n" ) );
	}

	// start up COM
	AfxOleInit();
	::CoInitialize( NULL );

	// reference to GDI+
	InitGdiplus();

	// crawl through directory tree defined by the command line
	// parameter trolling for image files
	RecursePath( csPath );

	// clean up references to GDI+
	TerminateGdiplus();

	// all is good
	return 0;

} // _tmain

/////////////////////////////////////////////////////////////////////////////
