/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kfmclient_h
#define __kfmclient_h

#include <kapp.h>
#include <krun.h>

class clientApp : public KApplication
{
  Q_OBJECT
public:

  clientApp( )
    : KApplication( false /*no styles*/ )
    { }

  ~clientApp() {}

  /** Parse command-line arguments and "do it" */
  bool doIt();

  /** Make konqueror open a window for @p url */
  bool createNewWindow(const KURL & url);

  /** Make konqueror open a window for @p profile and @p url */
  bool openProfile(const QString & profile, const QString & url);

protected slots:
  void slotResult( KIO::Job * );
  void slotAppRegistered( const QCString & );
  void slotDeref();
  void delayedQuit();

private:
  QString m_profileName;
  QString m_url;
  bool m_ok;
};

#endif
