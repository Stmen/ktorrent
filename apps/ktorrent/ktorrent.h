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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ***************************************************************************/


#ifndef _KTORRENT_H_
#define _KTORRENT_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kmainwindow.h>
#include <qtimer.h>
#include <interfaces/guiinterface.h>

typedef QValueList<QCString> QCStringList;

class KPrinter;
class KAction;
class KToggleAction;
class KURL;
class KTorrentCore;
class KTorrentView;
class TrayIcon;
class SetMaxRate;
class KTabWidget;
class KTorrentDCOP;
class QLabel;
class QListViewItem;
class KTorrentPreferences;



namespace kt
{
	class TorrentInterface;
	class ExpandableWidget;
}

/**
 * This class serves as the main window for KTorrent.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Joris Guisson <joris.guisson@gmail.com>
 * @version 0.1
 */
class KTorrent : public KMainWindow, public kt::GUIInterface
{
	Q_OBJECT
public:
	/**
	 * Default Constructor
	 */
	KTorrent();

	/**
	 * Default Destructor
	 */
	virtual ~KTorrent();


	KTorrentCore & getCore() {return *m_core;}
	
	/**
	 * Apply the settings.
	 * @param change_port Wether or not to change the server port
	 */
	void applySettings(bool change_port = true);

	virtual void addTabPage(QWidget* page,const QIconSet & icon,const QString & caption);
	virtual void removeTabPage(QWidget* page);
	virtual void addPrefPage(kt::PrefPageInterface* page);
	virtual void removePrefPage(kt::PrefPageInterface* page);
	virtual void mergePluginGui(kt::Plugin* p);
	virtual void removePluginGui(kt::Plugin* p);
	virtual void addWidgetBelowView(QWidget* w);
	virtual void removeWidgetBelowView(QWidget* w);
	virtual const kt::TorrentInterface* getCurrentTorrent() const;
	
	KTorrentView* getCurrentView();
	kt::PanelView getCurrentPanel();

	QString	getStatusInfo();
	QString	getStatusTransfer();
	QString	getStatusSpeed();
	QString	getStatusDHT();
	QCStringList getTorrentInfo(kt::TorrentInterface* tc);

public slots:
	/**
	 * Use this method to load whatever file/URL you have
	 */
	void load(const KURL& url);
	
	/**
	 * Does the same as load, but doesn't ask any questions
	*/
	void loadSilently(const KURL& url);

protected:
	/**
	 * This function is called when it is time for the app to save its
	 * properties for session management purposes.
	 */
	void saveProperties(KConfig *);

	/**
	 * This function is called when this app is restored.  The KConfig
	 * object points to the session management config file that was saved
	 * with @ref saveProperties
	 */
	void readProperties(KConfig *);


private slots:
	void fileOpen();
	void fileNew();
	void torrentPaste();
	void startDownload();
	void startAllDownloads();
	void stopDownload();
	void stopAllDownloads();
	void queueAction();
	void showIPFilter();
	void removeDownload();
	void queueManagerShow();
	void optionsShowStatusbar();
	void optionsConfigureKeys();
	void optionsConfigureToolbars();
	void optionsPreferences();
	void newToolbarConfig();
	void changeStatusbar(const QString& text);
	void changeCaption(const QString& text);
	void currentDownloadChanged(kt::TorrentInterface* tc);
	void currentSeedChanged(kt::TorrentInterface* tc);
	void updatedStats();
	void urlDropped(QDropEvent*,QListViewItem*);
	void currentTabChanged(QWidget* tab);
	void onUpdateActions(bool can_start,bool can_stop,bool can_remove,bool can_scan);
	void checkDataIntegrity();
	
private:
	void setupAccel();
	void setupActions();
	bool queryClose();
	bool queryExit();
	virtual void addWidgetInView(QWidget* w,kt::Position pos);
	virtual void removeWidgetFromView(QWidget* w);
	virtual void addWidgetInSeedView(QWidget* w,kt::Position pos);
	virtual void removeWidgetFromSeedView(QWidget* w);
	
	
private:
	KTorrentView *m_view;
	KTorrentView *m_seedView;
	KToggleAction *m_statusbarAction;
	
	KTorrentCore* m_core;
	TrayIcon* m_systray_icon;
	SetMaxRate* m_set_max_upload_rate;
	SetMaxRate* m_set_max_download_rate;
	KTabWidget* m_tabs;
	KTorrentDCOP* m_dcop;
	QTimer m_gui_update_timer;
	KTorrentPreferences* m_pref;
	
	kt::ExpandableWidget* m_exp;
	kt::ExpandableWidget* m_view_exp;
	kt::ExpandableWidget* m_seedView_exp;

	QLabel* m_statusInfo;
	QLabel* m_statusTransfer;
	QLabel* m_statusSpeed;
	QLabel* m_statusDHT;
	
	KAction* m_start;
	KAction* m_stop;
	KAction* m_remove;
	KAction* m_startall;
	KAction* m_stopall;
	KAction* m_pasteurl;
	KAction* m_queuemgr; 
	KAction* m_queueaction;
	KAction* m_datacheck;
	KAction* m_ipfilter;
};

#endif // _KTORRENT_H_
