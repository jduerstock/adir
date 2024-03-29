//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include "cdsk.h"

CDisk::CDisk()
{ 
	m_bHasData = FALSE; 
	m_pbtMemory = NULL;

	m_iMaxSectorWritten = 0;

	#ifdef _MEMORY_DUMP_
		printf( "CDisk constructed: %p\n", this );
	#endif
}

CDisk::~CDisk()
{
	if ( m_pbtMemory )
	{
		delete [] m_pbtMemory;
		m_pbtMemory = NULL;
	}

	#ifdef _MEMORY_DUMP_
		printf( "CDisk destructed: %p\n", this );
	#endif
}

BOOL CDisk::Duplicate( CDisk* pDisk, DISK_GEOMETRY* pGeoForced )
{
	DISK_GEOMETRY* pGeo = pGeoForced;
	if ( !pGeo )
		pGeo = &( pDisk->m_geometry );

	if ( !Format( pGeo ) )
		return FALSE;

	memcpy( m_pbtMemory, pDisk->m_pbtMemory, pDisk->m_iAllocated );
	m_iMaxSectorWritten = pDisk->m_iMaxSectorWritten;
	return TRUE;
}

BOOL CDisk::Format( DISK_GEOMETRY* pgeo )
{
	m_iMaxSectorWritten = 0;

	int iSectors = pgeo->iSides * pgeo->iTracks * pgeo->iSectorsPerTrack;

	if ( m_pbtMemory )
		delete [] m_pbtMemory;

	m_iAllocated = iSectors * pgeo->iBytesPerSector;
	m_pbtMemory = new BYTE [ m_iAllocated ];

	if ( !m_pbtMemory )
	{
		sprintf( m_szLastError, "DISK: Can't format - Not enough memory!" );
		return FALSE;
	}

	memset( m_pbtMemory, 0, m_iAllocated );

	m_geometry.iSides = pgeo->iSides;
	m_geometry.iTracks = pgeo->iTracks;
	m_geometry.iSectorsPerTrack = pgeo->iSectorsPerTrack;
	m_geometry.iBytesPerSector = pgeo->iBytesPerSector;
	m_geometry.iSectors = iSectors;

	return TRUE;
}

BOOL CDisk::ReadSector( void* pBuf, int iStartSec )
{
	if ( iStartSec && ( iStartSec <= m_geometry.iSectors ) )
	//Dufek: try to honor (return proper) size of first 3 sectors in 8 bit FDD Atari World
	//because of (at least seen in) boot sector processing in filesystem type guessing ...
	//where 3*128 !!! bytes is always allocated for boot area, even if next sectors are 256 bytes
	//and this program is killed by malloc structure checks in glibc on modern compilers/runtimes !!!
	//
	//I see no problem with author's choice to store all sectors fullsize in working memory
	//(WriteSector later here), but while reading, returned = copied size should be imho proper.
	//
	//AFAIK Atari works with first 3 sectors as 128 byte sectors always, although i saw some
	//mutation of ATR files storing all sectors 256 bytes beforehand. Mostly seen ATR files
	//and ATR software store first 3 sectors as 128 bytes and in CAtr::Load it is honored so ...
	//and in XFD format probably (afaik) too ...
	//Pavel Dufek, Czech Republic, 2022.
		memcpy( pBuf, m_pbtMemory + ( iStartSec - 1 ) * m_geometry.iBytesPerSector, ( iStartSec <= 3 ) ? 0x80: m_geometry.iBytesPerSector );
	else
	{
		sprintf( m_szLastError, "DISK: Reading non-existent sector: %04X", iStartSec );
		return FALSE;
	}

	return TRUE;
}

BOOL CDisk::WriteSector( int iStartSec, void* pBuf )
{
	//printf( " ws: %d ", iStartSec );
	if ( iStartSec && ( iStartSec <= m_geometry.iSectors ) )
		memcpy( m_pbtMemory + ( iStartSec - 1 ) * m_geometry.iBytesPerSector, pBuf, m_geometry.iBytesPerSector );
	else
	{
		sprintf( m_szLastError, "DISK: Writing non-existent sector: %04X", iStartSec );
		return FALSE;
	}

	if ( iStartSec > m_iMaxSectorWritten )
		m_iMaxSectorWritten = iStartSec;

	return TRUE;
}

