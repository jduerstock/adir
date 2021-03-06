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

#ifndef __CJONW_H__
#define __CJONW_H__

#include "wintypes.h"
#include "cfs.h"

class CJonwDirEntry : public CDirEntry
{
public:
	int m_iStartSec;
	int m_iSecCount;
};

class CJonw : public CFs
{
public:
	CJonw();
	~CJonw();

	BOOL	Mount( ADisk* );
	void	Dismount();
	BOOL	ExportFile( char*, CDirEntry* );

private:
	CJonwDirEntry* CreateEntry( BYTE, WORD, BYTE* );
};

#endif
