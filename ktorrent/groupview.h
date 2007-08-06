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
#ifndef KTGROUPVIEW_H
#define KTGROUPVIEW_H

#include <QTreeWidget>

class KMenu;
class KActionCollection;
class KSharedConfigPtr;

namespace kt
{
	class View;
	class Group;
	class GroupView;
	class GroupManager;
	class ViewManager;
		
	class GroupViewItem : public QTreeWidgetItem
	{
		Group* g;
	public:
		GroupViewItem(GroupView* parent,Group* g);
		GroupViewItem(QTreeWidgetItem* parent,Group* g);
		virtual ~GroupViewItem();
		
		Group* group() {return g;}
	//	virtual int compare(QListViewItem* i,int col,bool ascending) const; 
	};

	/**
		@author Joris Guisson <joris.guisson@gmail.com>
	*/
	class GroupView : public QTreeWidget
	{
		Q_OBJECT
	public:
		GroupView(GroupManager* gman,ViewManager* view,KActionCollection* col,QWidget *parent);
		virtual ~GroupView();
		
		/// Get the current group
		Group* currentGroup() {return current;} 

		/// Save the status of the group view
		void saveState(KSharedConfigPtr cfg);

		/// Load status from config
		void loadState(KSharedConfigPtr cfg);
		
	private slots:
		void onItemActivated(QTreeWidgetItem* item,int col);
		void onItemChanged(QTreeWidgetItem* item,int col);
		void showContextMenu(const QPoint & p);
		void addGroup();
		void removeGroup();
		void editGroupName();
		void openView();
		
		
	signals:
		void currentGroupChanged(kt::Group* g);
		void groupRenamed(kt::Group* g);
		void openNewTab(kt::Group* g);
		void groupRemoved(kt::Group* g);
		
	private:
		void createMenu(KActionCollection* col);
		GroupViewItem* addGroup(Group* g,QTreeWidgetItem* parent);
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent* event);

	private:
		ViewManager* view;
		QTreeWidgetItem* custom_root;
		GroupManager* gman;
		
		Group* current;
		GroupViewItem* current_item;

		KMenu* menu;
		KAction* new_group;
		KAction* edit_group;
		KAction* remove_group;
		KAction* open_in_new_tab;
		
		bool editing_item;
		
		friend class GroupViewItem;
	};

}

#endif
