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

#include "appsettings.h"

AppSettings::AppSettings(QObject *parent) :
    QObject(parent),
    m_useConnectionLess(true),
    m_nfcUri(LLCP_CONNECTIONORIENTED_SERVICENAME),
    m_nfcPort(LLCP_CONNECTIONLESS_PORT),
    m_sendThroughServerSocket(true),
    m_connectClientSocket(true),
    m_connectServerSocket(true)
{
    loadSettings();
}

void AppSettings::setUseConnectionLess(const bool useConnectionLess)
{
    if (useConnectionLess != m_useConnectionLess) {
        m_useConnectionLess = useConnectionLess;
        saveSettings();
    }
}

bool AppSettings::useConnectionLess()
{
    return m_useConnectionLess;
}

void AppSettings::setNfcUri(const QString &nfcUri)
{
    if (nfcUri != m_nfcUri) {
        m_nfcUri = nfcUri;
        saveSettings();
    }
}

QString AppSettings::nfcUri()
{
    return m_nfcUri;
}

void AppSettings::setNfcPort(const int nfcPort)
{
    if (nfcPort != m_nfcPort) {
        m_nfcPort = nfcPort;
        saveSettings();
    }
}

int AppSettings::nfcPort()
{
    return m_nfcPort;
}

void AppSettings::setSendThroughServerSocket(const bool sendThroughServerSocket)
{
    if (sendThroughServerSocket != m_sendThroughServerSocket) {
        m_sendThroughServerSocket = sendThroughServerSocket;
        saveSettings();
    }
}

bool AppSettings::sendThroughServerSocket()
{
    return m_sendThroughServerSocket;
}

void AppSettings::setConnectClientSocket(const bool connectClientSocket)
{
    if (connectClientSocket != m_connectClientSocket) {
        m_connectClientSocket = connectClientSocket;
        saveSettings();
    }
}

bool AppSettings::connectClientSocket()
{
    return m_connectClientSocket;
}

void AppSettings::setConnectServerSocket(const bool connectServerSocket)
{
    if (connectServerSocket != m_connectServerSocket) {
        m_connectServerSocket = connectServerSocket;
        saveSettings();
    }
}

bool AppSettings::connectServerSocket()
{
    return m_connectServerSocket;
}

void AppSettings::saveSettings()
{
    QSettings settings(SETTINGS_ORG, SETTINGS_APP, this);
    settings.setValue("useConnectionLess", m_useConnectionLess);
    settings.setValue("nfcUri", m_nfcUri);
    settings.setValue("nfcPort", m_nfcPort);
    settings.setValue("sendThroughServerSocket", m_sendThroughServerSocket);
    settings.setValue("connectClientSocket", m_connectClientSocket);
    settings.setValue("connectServerSocket", m_connectServerSocket);
}

void AppSettings::loadSettings()
{
    QSettings settings(SETTINGS_ORG, SETTINGS_APP, this);
    m_useConnectionLess = settings.value("useConnectionLess", true).toBool();
    m_nfcUri = settings.value("nfcUri", LLCP_CONNECTIONORIENTED_SERVICENAME).toString();
    m_nfcPort = settings.value("nfcPort", LLCP_CONNECTIONLESS_PORT).toInt();
    m_sendThroughServerSocket = settings.value("sendThroughServerSocket", true).toBool();
    m_connectClientSocket = settings.value("connectClientSocket", true).toBool();
    m_connectServerSocket = settings.value("connectServerSocket", true).toBool();
}
