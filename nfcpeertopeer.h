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

#ifndef NFCPEERTOPEER_H
#define NFCPEERTOPEER_H

#include <QObject>
#include <QDeclarativeView>
#include <QTimer>
#include <qnearfieldmanager.h>
#include <qllcpserver.h>
#include <qllcpsocket.h>
#include "appsettings.h"

QTM_USE_NAMESPACE   // Use Qt Mobility namespace

#ifdef Q_OS_SYMBIAN
#include "nfcsettings.h"
#endif

class NfcPeerToPeer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY busyChanged)

public:
    explicit NfcPeerToPeer(QObject *parent = 0);
    ~NfcPeerToPeer();

    void setAppSettings(AppSettings* appSettings);
    void setDeclarativeView(QDeclarativeView &view);

    bool isBusy() const;
signals:
    void nfcStatusUpdate(const QString& nfcStatusText);
    void nfcStatusError(const QString& nfcStatusErrorText);
    void chatMessage(const QString& nfcClientMessage);
    void statusMessage(const QString& statusMessage);
    void settingsApplied();
    void busyChanged();

public slots:
    void applySettings();
    void checkNfcStatus();
    void initAndStartNfc();
    void sendText(const QString& text);

private slots:
    void doApplySettings();

    void targetDetected(QNearFieldTarget *target);
    void targetLost(QNearFieldTarget *target);
    void targetError(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId &id);

    void handleNewConnection();
    void serverSocketError(QLlcpSocket::SocketError socketError);
    void serverSocketStateChanged ( QLlcpSocket::SocketState socketState );
    void serverSocketDisconnected ( );

    void readTextClient();
    void readTextServer();
    void clientSocketDisconnected();
    void clientSocketError(QLlcpSocket::SocketError socketError);
    void clientSocketStateChanged ( QLlcpSocket::SocketState socketState );

    void targetMessageDetected(const QNdefMessage &message, QNearFieldTarget *target);
private:
    void resetAll();
    void copyNfcUriFromAppSettings();
    void initClientSocket();
    void readText(QLlcpSocket *socket, const bool isServerSocket);
    bool sendCachedText();
    QString convertTargetErrorToString(QNearFieldTarget::Error error);
    QString convertSocketStateToString(QLlcpSocket::SocketState socketState);
    QString convertSocketErrorToString(QLlcpSocket::SocketError socketError);

#ifdef Q_OS_SYMBIAN
private slots:
    // Check for NFC Support
    void handleNfcError(NfcSettings::NfcError nfcError, int error);
    void handleNfcModeChange(NfcSettings::NfcMode nfcMode);
private:
    NfcSettings* m_nfcSettings;
#endif

private:
    AppSettings* m_appSettings;
    char* m_nfcUri;
    int m_nfcPort;
    QNearFieldManager *m_nfcManager;
    QNearFieldTarget *m_nfcTarget;
    QLlcpServer *m_nfcServer;
    QLlcpSocket *m_nfcClientSocket;
    QLlcpSocket *m_nfcServerSocket;
    QString m_sendTextQueue;
    bool m_isResetting;
    // Use connection-less or connection-oriented LLCP.
    // In case of connection-less, will connect to: m_nfcPort
    // For connection-oriented, connects to: m_nfcUri
    // If set to use connection-less mode, the other three settings are not used
    // (sendThroughServerSocket, connectClientSocket, connectServerSocket), as the
    // connection only goes through a single socket anyway.
    // Note: the value of this one has to be set when constructing the class and
    // can't be changed afterwards! (as sockets need to be initialized differently).
    bool m_useConnectionLess;
    // Send text through the server socket or through the client socket.
    // Only used for connection-oriented mode.
    bool m_sendThroughServerSocket;
    // Connect the client socket once a suitable target is found.
    // Only used for connection-oriented mode.
    // Note: if sendThroughServerSocket is false, this setting has to be true.
    // Otherwise, the app would be unable to send data (can't send data through a
    // not-connected client socket).
    bool m_connectClientSocket;
    // Start a server to listen for incoming client connections.
    // Only used for connection-oriented mode.
    bool m_connectServerSocket;

    bool m_isBusy;

    /*! Needed on MeeGo Harmattan to raise the app to the foreground when
      it's autostarted. */
    QDeclarativeView* m_declarativeView;
};

#endif // NFCPEERTOPEER_H
