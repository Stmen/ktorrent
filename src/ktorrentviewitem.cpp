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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <klocale.h>
#include <libtorrent/torrentcontrol.h>
#include <qdatetime.h>
#include <math.h>
#include "ktorrentviewitem.h"

using namespace bt;


KTorrentViewItem::KTorrentViewItem(QListView* parent,bt::TorrentControl* tc)
	: KListViewItem(parent),tc(tc)
{}


KTorrentViewItem::~KTorrentViewItem()
{}

void KTorrentViewItem::update()
{
	float to_meg = (1024.0 * 1024.0);
	setText(0,tc->getTorrentName());
	setText(1,i18n("%1 MB / %2 MB")
			.arg((float)tc->getBytesDownloaded() / to_meg)
					.arg((float)tc->getTotalBytes()/ to_meg));
	
	setText(2,i18n("%1 MB").arg((float)tc->getBytesUploaded()/ to_meg));
	setText(3,i18n("%1 kB/sec").arg(tc->getDownloadRate() / 1024.0));
	setText(4,i18n("%1 kB/sec").arg(tc->getUploadRate() / 1024.0));
	
	if (tc->getDownloadRate() != 0)
	{
		Uint32 secs = (int)floor((float)tc->getBytesLeft() / (float)tc->getDownloadRate());
		QTime t;
		t = t.addSecs(secs);
		setText(5,t.toString("hh:mm:ss"));
	}
	else
	{
		setText(5,i18n("infinity"));
	}
	setText(6,QString::number(tc->getNumPeers()));
	setText(7,QString("%1 (%2) / %3")
			.arg(tc->getNumChunksDownloaded())
			.arg(tc->getNumChunksDownloading())
			.arg(tc->getTotalChunks()));
}

