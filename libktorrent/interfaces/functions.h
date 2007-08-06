/***************************************************************************
 *   Copyright (C) 2005 by Joris Guisson                                   *
 *   joris.guisson@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <qstring.h>
#include <util/constants.h>
#include <ktorrent_export.h>

namespace kt
{

	KTORRENT_EXPORT QString BytesToString(bt::Uint64 bytes,int precision = -1);
	KTORRENT_EXPORT QString KBytesPerSecToString(double speed,int precision = 1);
	KTORRENT_EXPORT QString DurationToString(bt::Uint32 nsecs);
	KTORRENT_EXPORT QString DataDir();
	KTORRENT_EXPORT void ApplySettings();

	/// Get the network interface which needs to be used (this will return the name e.g. eth0, wlan0 ...)
	KTORRENT_EXPORT QString NetworkInterface(); 

	/// Get the IP address of the network interface
	KTORRENT_EXPORT QString NetworkInterfaceIPAddress(const QString & iface);
			
	template<class T> int CompareVal(T a,T b)
	{
		if (a < b)
			return -1;
		else if (a > b)
			return 1;
		else
			return 0;
	}
}

#endif
