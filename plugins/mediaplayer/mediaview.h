/***************************************************************************
 *   Copyright (C) 2008 by Joris Guisson and Ivan Vasic                    *
 *   joris.guisson@gmail.com                                               *
 *   ivasic@gmail.com                                                      *
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

#ifndef KTMEDIAVIEW_H
#define KTMEDIAVIEW_H

#include <KSharedConfig>
#include <QCheckBox>
#include <QListView>
#include <QSortFilterProxyModel>

#include "mediafile.h"

class QLineEdit;
class KToolBar;

namespace kt
{
class MediaModel;

/**
 * QSortFilterProxyModel to filter out incomplete files
 */
class MediaViewFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    MediaViewFilter(QObject *parent = 0);
    ~MediaViewFilter() override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    /// Enable or disable showing of incomplete files
    void setShowIncomplete(bool on);

public Q_SLOTS:
    void refresh();

private:
    bool show_incomplete;
};

/**
    @author
*/
class MediaView : public QWidget
{
    Q_OBJECT
public:
    MediaView(MediaModel *model, QWidget *parent);
    ~MediaView() override;

    void saveState(KSharedConfig::Ptr cfg);
    void loadState(KSharedConfig::Ptr cfg);

Q_SIGNALS:
    void doubleClicked(const MediaFileRef &mf);

private Q_SLOTS:
    void onDoubleClicked(const QModelIndex &index);
    void showIncompleteChanged(bool on);

private:
    MediaModel *model;
    QListView *media_tree;
    QLineEdit *search_box;
    MediaViewFilter *filter;
    KToolBar *tool_bar;
    QAction *show_incomplete;
    QAction *refresh;
};

}

#endif
