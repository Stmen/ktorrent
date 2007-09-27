/***************************************************************************
 *   Copyright (C) 2007 by Joris Guisson                                   *
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
#ifndef KT_PEERVIEW_HH
#define KT_PEERVIEW_HH


#include <QTreeWidget>
#include <util/ptrmap.h>
#include <interfaces/peerinterface.h>

class KSharedConfig;
template<class T> class KSharedPtr;
typedef KSharedPtr<KSharedConfig> KSharedConfigPtr;

namespace kt
{
	class PeerView;

	/**
	 * Item for the PeerView
	 * */
	class PeerViewItem : public QTreeWidgetItem
	{
	public:
		PeerViewItem(PeerView* pv,PeerInterface* peer);
		virtual ~PeerViewItem();

		void update(bool init = false);

		bool operator < (const QTreeWidgetItem & other) const;
	private:
		PeerInterface* peer;
		PeerInterface::Stats stats;
	};


	/**
	 * View which shows a list of peers, of a torrent.
	 * */
	class PeerView : public QTreeWidget
	{
		Q_OBJECT
	public:
		PeerView(QWidget* parent);
		virtual ~PeerView();

		/// A peer has been added
		void peerAdded(PeerInterface* peer);

		/// A peer has been removed
		void peerRemoved(PeerInterface* peer);

		/// Check to see if the GUI needs to be updated
		void update();

		/// Remove all items
		void removeAll();
		
		void saveState(KSharedConfigPtr cfg);
		void loadState(KSharedConfigPtr cfg);
	private:
		bt::PtrMap<PeerInterface*,PeerViewItem> items;
	};
}

#endif
