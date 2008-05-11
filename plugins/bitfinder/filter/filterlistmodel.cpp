/***************************************************************************
 *   Copyright (C) 2008 by Alan Jones                                      *
 *   skyphyr@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include <kicon.h>

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

#include <util/log.h>

#include "filterdetails.h"
#include "filterlistmodel.h"

using namespace bt;

namespace kt
	{

	FilterListModel::FilterListModel ( QString configDirName, CoreInterface* core, GUIInterface* gui, QObject* parent )
			: QAbstractListModel ( parent ),configDirName(configDirName), core ( core ),gui ( gui )
		{
		//let's verify the settings directory exists
		QFileInfo configDir(configDirName);
		if (configDir.exists())
			{
			if (!configDir.isDir())
				{
				//it's a file :O
				//delete the file, then create the directory
				QFile vigilantie(configDirName);
				vigilantie.remove();
				QDir mkConfigDir;
				mkConfigDir.mkdir(configDirName);
				}
			}
		else
			{
			//doesn't exist - let's create it
			QDir mkConfigDir;
			mkConfigDir.mkdir(configDirName);
			}
		
		changeTimeout.setSingleShot(true);
		connect(&changeTimeout, SIGNAL(timeout()), this, SLOT(saveFilters()));
		connect(this, SIGNAL(newFilterAdded(const QModelIndex&)), this, SLOT(openFilterTab(const QModelIndex&)));
		}


	FilterListModel::~FilterListModel()
		{
		qDeleteAll ( filters );
		}

	int FilterListModel::rowCount ( const QModelIndex & parent ) const
		{
		if ( !parent.isValid() )
			return filters.count();
		else
			return 0;
		}

	QVariant FilterListModel::headerData ( int section, Qt::Orientation orientation,int role ) const
		{
		Q_UNUSED ( section );
		Q_UNUSED ( orientation );
		Q_UNUSED ( role );
		return QVariant();
		}

	QVariant FilterListModel::data ( const QModelIndex & index, int role ) const
		{
		if ( index.column() != 0 )
			return QVariant();

		Filter* filter = ( Filter* ) index.internalPointer();
		if ( filter )
			{
			switch ( role )
				{
				case Qt::ToolTipRole:
					break;
				case Qt::DisplayRole:
					return filter->getName();
					break;
				case Qt::DecorationRole:
					return KIcon ( filter->getIconName() );
					//case Qt::UserRole:

				default:
					return QVariant();
				}
			}
		else
			{
			Out(SYS_BTF|LOG_DEBUG) << "Filter pointer is no good. Failed to pull data." << endl;
			}

		return QVariant();
		}
	
	QModelIndex FilterListModel::index(int row,int column,const QModelIndex & parent) const
		{
		Q_UNUSED(parent);
		
		if (column != 0)
			return QModelIndex();
		
		if (row >= filters.count())
			return QModelIndex();
			
		return createIndex(row,column,filters.at(row));
		}
	
	QModelIndex FilterListModel::next ( const QModelIndex & idx ) const
		{
		int nextRow = idx.row() + 1;

		if ( nextRow >= filters.count() )
			return QModelIndex();

		Filter* filter = filters.at ( nextRow );

		return createIndex ( nextRow, 0, filter );
		}

	QModelIndex FilterListModel::previous ( const QModelIndex & idx ) const
		{
		int prevRow = idx.row() - 1;

		if ( prevRow < 0 )
			return QModelIndex();

		Filter* filter = filters.at ( prevRow );

		return createIndex ( prevRow, 0, filter );
		}

	void FilterListModel::unload()
		{
		//check all the filterDetail tabs to see if this one is there
		for (int i=0; i<filterDetailsList.count(); i++)
			{
			//close any tabs we have open
			FilterDetails * filterTab = filterDetailsList.at(i);
			filterDetailsList.removeAt(i);
			gui->removeTabPage(filterTab);
			delete filterTab;
			}
		
		//if changes have been made, but not saved - save them now
		if (changeTimeout.isActive())
			saveFilters();
		}

	void FilterListModel::saveFilters()
		{
		QDomDocument filterXml("BitFinderFilters");
		
		//grab the xml element for each of the filters
		for (int i=0; i<filters.count(); i++)
			{
			filterXml.appendChild(filters.at(i)->getXmlElement());
			}
			
		//try to save the configuration off
		QFile file(configDirName + "filters.xml");
		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			//may want to fire off a warning here
			Out(SYS_BTF|LOG_IMPORTANT) << "Failed to save filters config to " << configDirName << "filters.xml" << endl;
			return;
		}
		
		QTextStream out(&file);
		filterXml.save(out, 4);
		}
	
	void FilterListModel::resetChangeTimer()
		{
		//this will cause a save to be triggered after a change
		//if multiple changes occur during that time it will reset
		//Should it seem to save too often increase this number
		changeTimeout.start(20000);
		}

	Filter* FilterListModel::addNewFilter ( const QString& name )
		{
		//seeing we're altering the data we need to let things know about it
		beginInsertRows(QModelIndex(), filters.count(), filters.count());
		
		Filter * curFilter = new Filter(name);
		filters.append(curFilter);
		
		//and now we're done
		endInsertRows();
		
		connect(curFilter, SIGNAL(nameChanged(const QString&)), this, SLOT(emitDataChanged()));
		connect(curFilter, SIGNAL(typeChanged(int)), this, SLOT(emitDataChanged()));
		
		connect(curFilter, SIGNAL(changed()), this, SLOT(resetChangeTimer()));
		
		emit newFilterAdded(createIndex(filters.count()-1, 0, curFilter));
		
		return curFilter;
		}
	
	void FilterListModel::insertFilter(const QModelIndex& idx, Filter * filter)
		{
		//seeing we're altering the data we need to let things know about it
		beginInsertRows(QModelIndex(), idx.row(), idx.row());
		
		filters.insert(idx.row(), filter);
		
		//and now we're done
		endInsertRows();
		
		connect(filter, SIGNAL(nameChanged(const QString&)), this, SLOT(emitDataChanged()));
		connect(filter, SIGNAL(typeChanged(int)), this, SLOT(emitDataChanged()));
		
		connect(filter, SIGNAL(changed()), this, SLOT(resetChangeTimer()));
		}
	
	void FilterListModel::removeFilter(const QModelIndex& idx)
		{
		//altering data we need to let things know
		beginRemoveRows(QModelIndex(), idx.row(), idx.row());
		
		Filter* curFilter = (Filter*)idx.internalPointer();
		
		//check all the filterDetail tabs to see if this one is there
		for (int i=0; i<filterDetailsList.count(); i++)
			{
			if (filterDetailsList.at(i)->getFilter()==curFilter)
				{
				//this tab is for the filter we're removing
				FilterDetails * filterTab = filterDetailsList.at(i);
				filterDetailsList.removeAt(i);
				gui->removeTabPage(filterTab);
				delete filterTab;
				}
			}
		
		//find the filter, remove it from the list and tell it to deleteLater
		filters.removeAll(curFilter);
		curFilter->deleteLater();
		
		//done altering data
		endRemoveRows();
		
		}
	
	void FilterListModel::moveFilterDown(const QModelIndex& idx)
		{
		if (idx.row() == filters.count()-1)
			return;
		
		int curRow = idx.row();
		Filter * shiftMe = filters.takeAt(curRow+1);
		filters.insert(curRow, shiftMe);
		
		QModelIndex from = createIndex(curRow, 0, filters.at(curRow));
		QModelIndex to = createIndex(curRow+1, 0, filters.at(curRow+1));
		
		emit dataChanged(from, to);
		}
	
	void FilterListModel::moveFilterUp(const QModelIndex& idx)
		{
		if (idx.row() == 0)
			return;
		
		int curRow = idx.row();
		Filter * shiftMe = filters.takeAt(curRow-1);
		filters.insert(curRow, shiftMe);
		
		QModelIndex from = createIndex(curRow-1, 0, filters.at(curRow-1));
		QModelIndex to = createIndex(curRow, 0, filters.at(curRow));
		
		emit dataChanged(from, to);
		}
	
	void FilterListModel::openFilterTab(const QModelIndex& idx)
		{
		for (int i=0; i<filterDetailsList.count(); i++)
			{
			if (filterDetailsList.at(i)->getFilter() == (Filter*)idx.internalPointer())
				{
				gui->setTabCurrent(filterDetailsList.at(i));
				return;
				}
			}
		
		FilterDetails * filterTab = new FilterDetails(core);
		Filter * curFilter = (Filter*)idx.internalPointer();
		filterTab->setFilter(curFilter);
		gui->addTabPage(filterTab,curFilter->getIconName() ,curFilter->getName(),this);
		filterTab->refreshSizes();
		
		connect(filterTab, SIGNAL(nameChanged(const QString&)), this, SLOT(setTabName(const QString&)));
		connect(filterTab, SIGNAL(typeChanged(int)), this, SLOT(setTabIcon()));
		
		filterDetailsList.append(filterTab);
		}
	
	void FilterListModel::setTabName(const QString& name)
		{
		QWidget * tab = qobject_cast<QWidget*>(sender());
		
		if (!tab)
			{
			return;
			}
		
		gui->setTabText(tab, name);
		}
	
	void FilterListModel::setTabIcon()
		{
		FilterDetails * tab = qobject_cast<FilterDetails*>(sender());
		
		if (!tab)
			{
			return;
			}
		
		gui->setTabIcon(tab, tab->getFilter()->getIconName());
		}
	
	void FilterListModel::emitDataChanged()
		{
		Filter * curFilter = qobject_cast<Filter*>(sender());
		
		if (!curFilter)
			{
			return;
			}
		
		for (int i=0; i<filters.count(); i++)
			{
			if (filters.at(i) == curFilter)
				{
				//this is the filter which changed so let's emit dataChanged
				QModelIndex curIndex = createIndex(i, 0, curFilter);
				emit dataChanged(curIndex, curIndex);
				return;
				}
			}
		}
	
	void FilterListModel::tabCloseRequest (kt::GUIInterface* gui, QWidget* tab)
		{
		//Check through the list of Filter tabs
		foreach(FilterDetails * filterTab, filterDetailsList)
			{
			if (filterTab == tab)
				{
				filterDetailsList.removeAll(filterTab);
				gui->removeTabPage(filterTab);
				delete filterTab;
				}
			
			}
		
		}
	
	}
