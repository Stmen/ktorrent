/***************************************************************************
 *   Copyright (C) 2005-2007 by Joris Guisson                              *
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
#include <math.h>
#include <float.h>
#include <qdatetime.h>
#include <qcheckbox.h>
#include <kglobal.h>
#include <klocale.h>
#include <interfaces/functions.h>
#include <interfaces/torrentinterface.h>

#include "downloadedchunkbar.h"
#include "availabilitychunkbar.h"
#include "statustab.h"
		
namespace kt
{

	StatusTab::StatusTab(QWidget* parent) : QWidget(parent),curr_tc(0)
	{
		setupUi(this);
		// do not use hardcoded colors
		hdr_info->setBackgroundRole(QPalette::Mid);
		hdr_chunks->setBackgroundRole(QPalette::Mid);
		hdr_sharing->setBackgroundRole(QPalette::Mid);
		
		ratio_limit->setMinimum(0.0f);
		ratio_limit->setMaximum(100.0f);
		ratio_limit->setSingleStep(0.1f);
		connect(ratio_limit, SIGNAL(valueChanged(double)), this, SLOT(maxRatioChanged(double)));
		connect(use_ratio_limit, SIGNAL(toggled(bool)), this, SLOT( useRatioLimitToggled(bool)));
		
		time_limit->setMinimum(0.0f);
		time_limit->setMaximum(10000000.0f);
		time_limit->setSingleStep(0.05f);
		time_limit->setSpecialValueText(i18n("No limit"));
		connect(use_time_limit,SIGNAL(toggled(bool)), this,SLOT(useTimeLimitToggled(bool)));
		connect(time_limit,SIGNAL(valueChanged(double)), this, SLOT(maxTimeChanged(double)));
		
		int h = (int)ceil(fontMetrics().height()*1.25);
		downloaded_bar->setFixedHeight(h);
		availability_bar->setFixedHeight(h);

		// initialize everything with curr_tc == 0
		setEnabled(false);
		ratio_limit->setValue(0.00f);
		share_ratio->clear();
		tracker_status->clear();
		seeders->clear();
		leechers->clear();
		next_update_in->clear();
		avg_up_speed->clear();
		avg_down_speed->clear();
	}
	
	StatusTab::~StatusTab()
	{}

	void StatusTab::changeTC(kt::TorrentInterface* tc)
	{
		if (tc == curr_tc)
			return;
	
		curr_tc = tc;
	
		downloaded_bar->setTC(tc);
		availability_bar->setTC(tc);
		setEnabled(tc != 0);
		
		if (curr_tc)
		{
			float ratio = curr_tc->getMaxShareRatio();
			if(ratio > 0)
			{
				use_ratio_limit->setChecked(true);
				ratio_limit->setValue(ratio);
				ratio_limit->setEnabled(true);
			}
			else
			{
				ratio_limit->setValue(0.0);
				use_ratio_limit->setChecked(false);
				ratio_limit->setEnabled(false);
			}
			
			float hours = curr_tc->getMaxSeedTime();
			if (hours > 0)
			{
				time_limit->setEnabled(true);
				use_time_limit->setChecked(true);
				time_limit->setValue(hours);
			}
			else
			{
				time_limit->setEnabled(false);
				time_limit->setValue(0.0);
				use_time_limit->setChecked(false);
			}
		}
		else
		{
			ratio_limit->setValue(0.00f);
			time_limit->setValue(0.0);
			share_ratio->clear();
			tracker_status->clear();
			seeders->clear();
			leechers->clear();
			next_update_in->clear();
			avg_up_speed->clear();
			avg_down_speed->clear();
		}
		
		update();
	}
	
	void StatusTab::update()
	{
		if (!curr_tc)
			return;
	
		const TorrentStats & s = curr_tc->getStats();
		
		downloaded_bar->updateBar();
		availability_bar->updateBar();
		
		if (s.running)
		{
			QTime t;
			t = t.addSecs(curr_tc->getTimeToNextTrackerUpdate());
			next_update_in->setText(t.toString("mm:ss"));
		}
		else
		{
			next_update_in->clear();
		}
		
		tracker_status->setText(s.trackerstatus);
		
		seeders->setText(QString("%1 (%2)")
				.arg(s.seeders_connected_to).arg(s.seeders_total));
	
		leechers->setText(QString("%1 (%2)")
				.arg(s.leechers_connected_to).arg(s.leechers_total));
	
		float ratio = kt::ShareRatio(s);
		if(!ratio_limit->hasFocus() && use_ratio_limit->isChecked())
			maxRatioUpdate();
		
		share_ratio->setText(QString("<font color=\"%1\">%2</font>").arg(ratio <= 0.8 ? "#ff0000" : "#1c9a1c").arg(KGlobal::locale()->formatNumber(ratio,2)));
	
		Uint32 secs = curr_tc->getRunningTimeUL(); 
		if (secs == 0)
		{
			avg_up_speed->setText(KBytesPerSecToString(0));
		}
		else
		{
			double r = (double)s.bytes_uploaded / 1024.0;
			avg_up_speed->setText(KBytesPerSecToString(r / secs));
		}
		
		secs = curr_tc->getRunningTimeDL();
		if (secs == 0)
		{
			avg_down_speed->setText(KBytesPerSecToString(0));
		}
		else
		{
			double r = (double)(s.bytes_downloaded - s.imported_bytes)/ 1024.0;
			avg_down_speed->setText(KBytesPerSecToString(r / secs));
		}
	}
	
	void StatusTab::maxRatioChanged(double v)
	{
		if(!curr_tc)
			return;
		
		curr_tc->setMaxShareRatio(v);
	}
	
	void StatusTab::useRatioLimitToggled(bool state)
	{
		if(!curr_tc)
			return;
		
		ratio_limit->setEnabled(state);
		if (!state)
		{
			curr_tc->setMaxShareRatio(0.00f);
			ratio_limit->setValue(0.00f);
		}
		else
		{
			float msr = curr_tc->getMaxShareRatio();
			if(msr == 0.00f)
			{	
				curr_tc->setMaxShareRatio(1.00f);
				ratio_limit->setValue(1.00f);
			}
			
			float sr = kt::ShareRatio(curr_tc->getStats());
			if(sr >= 1.00f)
			{
				//always add 1 to max share ratio to prevent stopping if torrent is running.
				curr_tc->setMaxShareRatio(sr + 1.00f);
				ratio_limit->setValue(sr + 1.00f);
			}
		}
	}
	
	void StatusTab::maxRatioUpdate()
	{
		if(!curr_tc)
			return;
		
		float ratio = curr_tc->getMaxShareRatio();
		if(ratio > 0.00f)
		{
			ratio_limit->setEnabled(true);
			use_ratio_limit->setChecked(true);
			ratio_limit->setValue(ratio);
		}
		else
		{
			ratio_limit->setEnabled(false);
			use_ratio_limit->setChecked(false);
			ratio_limit->setValue(0.00f);
		}
	}
	
	void StatusTab::useTimeLimitToggled(bool on)
	{
		if(!curr_tc)
			return;
		
		time_limit->setEnabled(on);
		if (on)
		{
			Uint32 dl = curr_tc->getRunningTimeDL();
			Uint32 ul = curr_tc->getRunningTimeUL();
			float hours = (ul - dl) / 3600.0f + 1.0; // add one hour to current seed time to not stop immediatly
			time_limit->setValue(hours); 
			curr_tc->setMaxSeedTime(hours);
		}
		else
		{
			curr_tc->setMaxSeedTime(0.0f);
		}
	}
	
	void StatusTab::maxTimeChanged(double v)
	{
		if (curr_tc)
			curr_tc->setMaxSeedTime(v);
	}

}

#include "statustab.moc"

