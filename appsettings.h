/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Andreas Jakl (andreas.jakl@mopius.com)
**
** This file is part of an NFC tutorial for Qt Mobility.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDebug>

/*! Default connection URN for the connection-oriented mode. */
#define LLCP_CONNECTIONORIENTED_SERVICENAME "urn:nfc:xsn:nokia.com:nfcchat"
/*! Default connection port for connection-less mode. */
#define LLCP_CONNECTIONLESS_PORT    35
// Used for the QSettings file, for stats and IAP
#define SETTINGS_ORG "Nokia"
#define SETTINGS_APP "NfcChat"

class AppSettings : public QObject
{
    Q_OBJECT
public:
    explicit AppSettings(QObject *parent = 0);
    
signals:
    
public slots:
    void setUseConnectionLess(const bool useConnectionLess);
    bool useConnectionLess();
    void setNfcUri(const QString& nfcUri);
    QString nfcUri();
    void setNfcPort(const int nfcPort);
    int nfcPort();
    void setSendThroughServerSocket(const bool sendThroughServerSocket);
    bool sendThroughServerSocket();
    void setConnectClientSocket(const bool connectClientSocket);
    bool connectClientSocket();
    void setConnectServerSocket(const bool connectServerSocket);
    bool connectServerSocket();

private:
    void saveSettings();
    void loadSettings();

private:
    bool m_useConnectionLess;
    QString m_nfcUri;
    int m_nfcPort;
    bool m_sendThroughServerSocket;
    bool m_connectClientSocket;
    bool m_connectServerSocket;
};

#endif // APPSETTINGS_H
