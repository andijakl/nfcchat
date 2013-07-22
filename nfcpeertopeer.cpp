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

#include "nfcpeertopeer.h"

NfcPeerToPeer::NfcPeerToPeer(QObject *parent) :
    QObject(parent),
    m_appSettings(NULL),
    m_nfcUri(NULL),
    m_nfcManager(NULL),
    m_nfcTarget(NULL),
    m_nfcServer(NULL),
    m_nfcClientSocket(NULL),
    m_nfcServerSocket(NULL),
    m_isResetting(false),
    m_isBusy(false)
{
}

NfcPeerToPeer::~NfcPeerToPeer()
{
    delete[] m_nfcUri;
}

void NfcPeerToPeer::setAppSettings(AppSettings *appSettings)
{
    if (!m_appSettings) {
        m_appSettings = appSettings;
        m_appSettings->setParent(this);
        // Copy the values of the settings to the currently used config.
        // No need to call applySettings() yet, as this method is only
        // called before the NFC system is initialized, so we don't
        // need to restart any servers.
        m_useConnectionLess = m_appSettings->useConnectionLess();
        m_nfcPort = m_appSettings->nfcPort();
        copyNfcUriFromAppSettings() ;
        m_connectClientSocket = m_appSettings->connectClientSocket();
        m_connectServerSocket = m_appSettings->connectServerSocket();
        m_sendThroughServerSocket = m_appSettings->sendThroughServerSocket();
    }
}

void NfcPeerToPeer::applySettings()
{
    m_isResetting = true;
    m_isBusy = true;
    emit busyChanged();
    QTimer::singleShot(500, this, SLOT(doApplySettings()));
}

void NfcPeerToPeer::doApplySettings()
{
    qDebug(__PRETTY_FUNCTION__);
    if (!(m_isResetting && m_isBusy)) {
        // Allow running this method only if called through applySettings().
        return;
    }

    // This setting doesn't need any restarting of services,
    // so can always be copied from the settings object
    // to the currently used config.
    m_sendThroughServerSocket = m_appSettings->sendThroughServerSocket();

    // Now check for individual changes that affect the
    // communication and in most cases requires restarting services.
    if (m_appSettings->useConnectionLess() != m_useConnectionLess)
    {
        // Switched between connection-oriented and connection-less mode
        m_useConnectionLess = m_appSettings->useConnectionLess();
        // Close all current connections
        resetAll();
        // Start services again
        initAndStartNfc();
    }
    else if (m_useConnectionLess && m_appSettings->nfcPort() != m_nfcPort)
    {
        // Connection-less mode
        // Changed port
        m_nfcPort = m_appSettings->nfcPort();
        // Close all current connections
        resetAll();
        // Start services again
        initAndStartNfc();
    }
    else if (!m_useConnectionLess)
    {
        // Connection-oriented mode
        bool restartOrShutdownServer = false;
        bool restartOrShutdownClient = false;
        // Connection-oriented: check if URI changed
        if (m_appSettings->nfcUri() != QString(m_nfcUri)) {
            // New URI - restart socket server
            copyNfcUriFromAppSettings();
            restartOrShutdownServer = true;
            restartOrShutdownClient = true;
        }
        if (m_appSettings->connectClientSocket() != m_connectClientSocket) {
            m_connectClientSocket = m_appSettings->connectClientSocket();
            restartOrShutdownClient = true;
        }
        if (m_appSettings->connectServerSocket() != m_connectServerSocket) {
            m_connectServerSocket = m_appSettings->connectServerSocket();
            restartOrShutdownServer = true;
        }
        if (restartOrShutdownServer || restartOrShutdownClient) {
            // We could also differentiate here which socket to
            // re-establish, based on the two boolean variables.
            // But for now, just reset everything.
            // Close all current connections
            resetAll();
            // Start services again
            initAndStartNfc();
        }
    }
    m_isResetting = false;
    m_isBusy = false;
    emit busyChanged();
    emit settingsApplied();
}

void NfcPeerToPeer::copyNfcUriFromAppSettings()
{
    QByteArray nfcUriBA = m_appSettings->nfcUri().toLatin1();
    delete[] m_nfcUri;
    m_nfcUri = new char[nfcUriBA.size() + 1];
    strcpy(m_nfcUri, nfcUriBA.data());
}

void NfcPeerToPeer::resetAll()
{
    qDebug(__PRETTY_FUNCTION__);
    if (m_nfcClientSocket) {
        // close() also calls disconnectFromService().
        m_nfcClientSocket->close();
        m_nfcClientSocket->deleteLater();
        m_nfcClientSocket = NULL;
    }
    if (m_nfcServerSocket) {
        m_nfcServerSocket->close();
        m_nfcServerSocket->deleteLater();
        m_nfcServerSocket = NULL;
    }
    if (m_nfcServer) {
        m_nfcServer->close();
        m_nfcServer->deleteLater();
        m_nfcServer = NULL;
    }
    qDebug() << "NfcPeerToPeer::resetAll() finished";
}

void NfcPeerToPeer::initAndStartNfc()
{
    qDebug(__PRETTY_FUNCTION__);
    if (!m_appSettings) {
        return;
    }

    // Create the Near Field Manager and connect its main signals
    if (!m_nfcManager) {
        m_nfcManager = new QNearFieldManager(this);

        connect(m_nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)),
                this, SLOT(targetDetected(QNearFieldTarget*)));
        connect(m_nfcManager, SIGNAL(targetLost(QNearFieldTarget*)),
                this, SLOT(targetLost(QNearFieldTarget*)));

        m_nfcManager->setTargetAccessModes(QNearFieldManager::NdefReadTargetAccess | QNearFieldManager::NdefWriteTargetAccess);

        // Required for autostart tags
        m_nfcManager->registerNdefMessageHandler(QNdefRecord::ExternalRtd, "nokia.com:nfcchat", this, SLOT(targetMessageDetected(QNdefMessage,QNearFieldTarget*)));

        // Only detect other NFC devices. Leave the phone to handle NFC tags.
        // Start detecting targets
        bool activationSuccessful = m_nfcManager->startTargetDetection(QNearFieldTarget::NfcForumDevice);
        if (activationSuccessful) {
            emit nfcStatusUpdate("Successfully started target detection");
        } else {
            emit nfcStatusError("Error starting NFC target detection");
        }
    }

    // Create the NFC server which will listen for incoming connections
    // (for connection-oriented only, connectionless will bind the client
    // socket to the port).
    if (!m_useConnectionLess && m_connectServerSocket) {
        qDebug() << "Creating LLCP server";
        m_nfcServer = new QLlcpServer(this);
        connect(m_nfcServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));

        qDebug() << "Starting to listen to " << m_nfcUri;
        m_nfcServer->listen(m_nfcUri);
    }

    // The NFC client socket
    initClientSocket();
}

void NfcPeerToPeer::initClientSocket()
{
    if (!m_useConnectionLess && !m_connectClientSocket) {
        // If using only one socket for connection-oriented, do not initialize
        // a client (second) socket here. The server will already wait for incoming
        // connections.
        return;
    }

    if (m_nfcClientSocket) {
        delete m_nfcClientSocket;
    }

    qDebug() << "Creating new client socket";
    // The NFC client socket (in case of connectionless the only one)
    m_nfcClientSocket = new QLlcpSocket(this);
    connect(m_nfcClientSocket, SIGNAL(readyRead()), this, SLOT(readTextClient()));
    connect(m_nfcClientSocket, SIGNAL(disconnected()), this, SLOT(clientSocketDisconnected()));
    connect(m_nfcClientSocket, SIGNAL(error(QLlcpSocket::SocketError)), this, SLOT(clientSocketError(QLlcpSocket::SocketError)));
    connect(m_nfcClientSocket, SIGNAL(stateChanged(QLlcpSocket::SocketState)), this, SLOT(clientSocketStateChanged(QLlcpSocket::SocketState)));
    //connect(nfcClientSocket, SIGNAL(bytesWritten(qint64)))

    if (m_useConnectionLess) {
        m_nfcClientSocket->bind(m_nfcPort);
    } else {
        qDebug() << "Connecting client socket to service";
        #if defined(MEEGO_EDITION_HARMATTAN)
            // On Harmattan, already connect the client socket to the URI.
            // On Symbian, the connection can also be opened when the target is found.
            m_nfcClientSocket->connectToService(0, m_nfcUri);
        #endif
    }
    qDebug() << "NfcPeerToPeer::initClientSocket() finished";
}

void NfcPeerToPeer::checkNfcStatus()
{
#ifdef Q_OS_SYMBIAN
    // Construct a new instance.
    m_nfcSettings = new NfcSettings(this);

    // Retrieve the NFC feature support information.
    NfcSettings::NfcFeature nfcFeature = m_nfcSettings->nfcFeature();

    if (nfcFeature == NfcSettings::NfcFeatureSupported) {
        // Connect signals for receiving mode change and error notifications.
        connect(m_nfcSettings, SIGNAL(nfcModeChanged(NfcSettings::NfcMode)), SLOT(handleNfcModeChange(NfcSettings::NfcMode)));
        connect(m_nfcSettings, SIGNAL(nfcErrorOccurred(NfcSettings::NfcError, int)), SLOT(handleNfcError(NfcSettings::NfcError, int)));

        // Retrieve the initial value of the NFC mode setting.
        NfcSettings::NfcMode nfcMode = m_nfcSettings->nfcMode();

        if (nfcMode != NfcSettings::NfcModeOn) {
            // NFC is supported but not switched on, prompt the user to enable it.
            emit nfcStatusError(tr("NFC hardware is available but currently switched off."));
        } else {
            emit nfcStatusUpdate(tr("NFC is supported and switched on."));
        }
    }
    else if (nfcFeature == NfcSettings::NfcFeatureSupportedViaFirmwareUpdate) {
        // Display message to user to update device firmware
        emit nfcStatusError(tr("Update device firmware to enable NFC support."));
        return;
    } else {
        // Display message informing the user that NFC is not supported by this device.
        emit nfcStatusError(tr("NFC not supported by this device."));
        return;
    }
#endif
}

void NfcPeerToPeer::targetDetected(QNearFieldTarget *target)
{
    emit statusMessage("Target detected");
    // Cache target
    m_nfcTarget = target;
    // Check if the target supports LLCP access
    QNearFieldTarget::AccessMethods accessMethods = target->accessMethods();
    if (accessMethods.testFlag(QNearFieldTarget::LlcpAccess))
    {
        connect(target, SIGNAL(error(QNearFieldTarget::Error,QNearFieldTarget::RequestId)),
                this, SLOT(targetError(QNearFieldTarget::Error,QNearFieldTarget::RequestId)));

        if (!m_useConnectionLess && m_connectClientSocket) {
            #ifndef MEEGO_EDITION_HARMATTAN
                // Connect to the service on Symbian
                // (on Harmattan, the connection was already established at the beginning)
                m_nfcClientSocket->connectToService(target, m_nfcUri);
                emit statusMessage("Connecting to service...");
            #endif
        }
    }
}

void NfcPeerToPeer::targetLost(QNearFieldTarget *target)
{
    emit statusMessage("Target lost");
    if (!m_useConnectionLess && m_nfcClientSocket) {
        // Connection-oriented
        m_nfcClientSocket->disconnectFromService();
    } else {
        // Connection-less
#ifdef Q_OS_SYMBIAN
        // Delete and create a new client socket. Otherwise, we'd get an error from
        // the socket and it wouldn't work for new connections anymore.
        initClientSocket();
#endif
    }
    target->deleteLater();
    target = NULL;
}


void NfcPeerToPeer::handleNewConnection()
{
    if (!m_connectServerSocket)
        return;

    if (m_nfcServerSocket) {
        m_nfcServerSocket->deleteLater();
    }

    // The socket is a child of the server and will therefore be deleted automatically
    m_nfcServerSocket = m_nfcServer->nextPendingConnection();

    connect(m_nfcServerSocket, SIGNAL(readyRead()), this, SLOT(readTextServer()));
    connect(m_nfcServerSocket, SIGNAL(error(QLlcpSocket::SocketError)), this, SLOT(serverSocketError(QLlcpSocket::SocketError)));
    connect(m_nfcServerSocket, SIGNAL(stateChanged(QLlcpSocket::SocketState)), this, SLOT(serverSocketStateChanged(QLlcpSocket::SocketState)));
    connect(m_nfcServerSocket, SIGNAL(disconnected()), this, SLOT(serverSocketDisconnected()));

    emit statusMessage("New server socket connection");
    sendCachedText();
}

void NfcPeerToPeer::readTextClient()
{
    readText(m_nfcClientSocket, false);
}

void NfcPeerToPeer::readTextServer()
{
    readText(m_nfcServerSocket, true);
}

void NfcPeerToPeer::readText(QLlcpSocket* socket, const bool isServerSocket)
{
    if (!socket)
        return;

    bool hasDatagramWaiting = socket->hasPendingDatagrams();
    if (hasDatagramWaiting)
    {
        qint64 datagramSize = socket->pendingDatagramSize();
        char* rawData = new char[datagramSize];
        socket->readDatagram ( rawData, datagramSize );
        QString data = QString::fromUtf8(rawData, datagramSize);
        delete rawData;
        QString dataLength;
        dataLength.setNum(datagramSize);
        QString message = (isServerSocket ? "Server" : "Client");
        message.append(" (" + dataLength + "): " + data);
        emit chatMessage(message);
    } else {
        QByteArray rawData = socket->readAll();
        QString data = QString::fromUtf8(rawData.constData(), rawData.size());
        QString message = (isServerSocket ? "Server" : "Client");
        message.append(": " + data);
        emit chatMessage(message);
    }
}

void NfcPeerToPeer::sendText(const QString& text)
{
    bool textQueuedBefore = m_sendTextQueue.isEmpty() ? false : true;

    m_sendTextQueue = text;
    if (!sendCachedText()) {
        if (textQueuedBefore) {
            emit statusMessage("Enqueued message replaced");
        } else {
            emit statusMessage("Message enqueued");
        }
    }
}

bool NfcPeerToPeer::sendCachedText()
{
    if (!m_sendTextQueue.isEmpty()) {

        if (m_useConnectionLess) {
            // Connection-less
            if (m_nfcClientSocket) {
                // Connection-less doesn't have a server, only uses the client socket
                QByteArray utf8Text = m_sendTextQueue.toUtf8();
                m_nfcClientSocket->writeDatagram(utf8Text.data(), (qint64)utf8Text.size(), m_nfcTarget, m_nfcPort);
                m_sendTextQueue = QString();
                emit statusMessage("Datagram sent");
                return true;
            }
        }
        else {
            // Connection-oriented
            bool messageSent = false;
            if (m_sendThroughServerSocket && m_nfcServerSocket && m_nfcServerSocket->isOpen() && m_nfcServerSocket->isWritable()) {
                m_nfcServerSocket->write(m_sendTextQueue.toUtf8());
                messageSent = true;
            } else if (!m_sendThroughServerSocket && m_nfcClientSocket && m_nfcClientSocket->isOpen()){
                m_nfcClientSocket->write(m_sendTextQueue.toUtf8());
                messageSent = true;
            }
            if (messageSent) {
                m_sendTextQueue = QString();
                emit statusMessage("Message sent");
                return true;
            }
        }
    }
    return false;
}

void NfcPeerToPeer::clientSocketDisconnected()
{
    emit statusMessage("Client socket disconnected");
#ifdef MEEGO_EDITION_HARMATTAN
    if (!m_isResetting && !m_useConnectionLess && m_connectClientSocket) {
        m_nfcClientSocket->connectToService(0, m_nfcUri);
    }
#endif
}

void NfcPeerToPeer::serverSocketDisconnected()
{
    emit statusMessage("Server socket disconnected");
#ifdef MEEGO_EDITION_HARMATTAN
    if (!m_isResetting && m_nfcServerSocket) {
        m_nfcServerSocket->deleteLater();
        m_nfcServerSocket = NULL;
    }
#endif
}

void NfcPeerToPeer::serverSocketError(QLlcpSocket::SocketError socketError)
{
    emit statusMessage("Server socket error: " + convertSocketErrorToString(socketError));
}

void NfcPeerToPeer::clientSocketError(QLlcpSocket::SocketError socketError)
{
    // While resetting the sockets, error smight occur. Hide them from the UI.
    if (!m_isResetting) {
        emit statusMessage("Client socket error: " + convertSocketErrorToString(socketError));
    } else {
        qDebug() << "Client socket error: " + convertSocketErrorToString(socketError);
    }
}

void NfcPeerToPeer::serverSocketStateChanged(QLlcpSocket::SocketState socketState)
{
    emit statusMessage("Server socket state: " + convertSocketStateToString(socketState));
}

void NfcPeerToPeer::clientSocketStateChanged(QLlcpSocket::SocketState socketState)
{
    emit statusMessage("Client socket state: " + convertSocketStateToString(socketState));
    if (socketState == QLlcpSocket::ConnectedState) {
        sendCachedText();
    }
}

void NfcPeerToPeer::targetError(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId &/*id*/)
{
    emit statusMessage("Error: " + convertTargetErrorToString(error));
}


QString NfcPeerToPeer::convertTargetErrorToString(QNearFieldTarget::Error error)
{
    QString errorString = "Unknown";
    switch (error)
    {
    case QNearFieldTarget::NoError:
        errorString = "No error has occurred.";
        break;
    case QNearFieldTarget::UnsupportedError:
        errorString = "The requested operation is unsupported by this near field target.";
        break;
    case QNearFieldTarget::TargetOutOfRangeError:
        errorString = "The target is no longer within range.";
        break;
    case QNearFieldTarget::NoResponseError:
        errorString = "The target did not respond.";
        break;
    case QNearFieldTarget::ChecksumMismatchError:
        errorString = "The checksum has detected a corrupted response.";
        break;
    case QNearFieldTarget::InvalidParametersError:
        errorString = "Invalid parameters were passed to a tag type specific function.";
        break;
    case QNearFieldTarget::NdefReadError:
        errorString = "Failed to read NDEF messages from the target.";
        break;
    case QNearFieldTarget::NdefWriteError:
        errorString = "Failed to write NDEF messages to the target.";
        break;
    case QNearFieldTarget::UnknownError:
        errorString = "Unknown error.";
        break;
    }
    return errorString;
}

QString NfcPeerToPeer::convertSocketStateToString(QLlcpSocket::SocketState socketState)
{
    QString stateText = "Unknown";
    switch (socketState)
    {
    case QLlcpSocket::UnconnectedState:
        stateText = "The socket is not connected.";
        break;
    case QLlcpSocket::ConnectingState:
        stateText = "The socket has started establishing a connection.";
        break;
    case QLlcpSocket::ConnectedState:
        stateText = "A connection is established.";
        break;
    case QLlcpSocket::ClosingState:
        stateText = "The socket is about to close.";
        break;
    case QLlcpSocket::BoundState:
        stateText = "The socket is bound to a local port (for servers).";
        break;
    case QLlcpSocket::ListeningState:
        stateText = "The socket is listening for incoming connections (for internal use).";
        break;
    }
    return stateText;
}

QString NfcPeerToPeer::convertSocketErrorToString(QLlcpSocket::SocketError socketError)
{
    QString errorText = "Unknown";
    switch (socketError)
    {
    case QLlcpSocket::UnknownSocketError:
        errorText = "An unidentified error has occurred.";
        break;
    case QLlcpSocket::RemoteHostClosedError:
        errorText = "The remote host closed the connection.";
        break;
    case QLlcpSocket::SocketAccessError:
        errorText = "The socket operation failed because the application lacked the required privileges.";
        break;
    case QLlcpSocket::SocketResourceError:
        errorText = "The local system ran out of resources (e.g., too many sockets).";
        break;
    }
    return errorText;
}

#ifdef Q_OS_SYMBIAN
void NfcPeerToPeer::handleNfcModeChange(NfcSettings::NfcMode nfcMode)
{
    switch (nfcMode) {
    case NfcSettings::NfcModeNotSupported:
        // NFC is not currently supported. It is not possible to distinguish
        // whether a firmware update could enable NFC features based solely
        // on the value of the nfcMode parameter. The return value of
        // NfcSettings::nfcFeature() indicates whether a firmware update is
        // applicable to this device.
        emit nfcStatusError(tr("NFC is not currently supported."));
        break;
    case NfcSettings::NfcModeUnknown:
        // NFC is supported, but the current mode is unknown at this time.
        emit nfcStatusError(tr("NFC is supported, but the current mode is unknown at this time."));
        break;
    case NfcSettings::NfcModeOn:
        // NFC is supported and switched on.
        emit nfcStatusUpdate(tr("NFC is supported and switched on."));
        break;
    case NfcSettings::NfcModeCardOnly:
        // NFC hardware is available and currently in card emulation mode.
        emit nfcStatusError(tr("NFC hardware is available and currently in card emulation mode."));
        break;
    case NfcSettings::NfcModeOff:
        // NFC hardware is available but currently switched off.
        emit nfcStatusError(tr("NFC hardware is available but currently switched off."));
        break;
    default:
        break;
    }
}


void NfcPeerToPeer::handleNfcError(NfcSettings::NfcError nfcError, int error)
{
    // The platform specific error code is ignored here.
    Q_UNUSED(error)

    switch (nfcError) {
    case NfcSettings::NfcErrorFeatureSupportQuery:
        // Unable to query NFC feature support.
        emit nfcStatusError(tr("Unable to query NFC feature support."));
        break;
    case NfcSettings::NfcErrorSoftwareVersionQuery:
        // Unable to query device software version.
        emit nfcStatusError(tr("Unable to query device software version."));
        break;
    case NfcSettings::NfcErrorModeChangeNotificationRequest:
        // Unable to request NFC mode change notifications.
        emit nfcStatusError(tr("Unable to request NFC mode change notifications."));
        break;
    case NfcSettings::NfcErrorModeChangeNotification:
        // NFC mode change notification was received, but caused an error.
        emit nfcStatusError(tr("NFC mode change notification was received, but caused an error."));
        break;
    case NfcSettings::NfcErrorModeRetrieval:
        // Unable to retrieve current NFC mode.
        emit nfcStatusError(tr("Unable to retrieve current NFC mode."));
        break;
    default:
        break;
    }
}


#endif


/*!
  \brief Slot needed for the registerNdefMessageHandler() method.
  */
void NfcPeerToPeer::targetMessageDetected(const QNdefMessage &message, QNearFieldTarget* target)
{
    // We don't parse the NDEF message here, as this is only a
    // LLCP peer to peer example and doesn't need to read the contents
    // of the autostart tag.
    qDebug() << "NDEF Message detected (-> Autostart)!";
#ifdef MEEGO_EDITION_HARMATTAN
    // MeeGo: raise the app to the foreground in case it was autostarted
    // by touching the tag AND it was already running in the background.
    // If we wouldn't do it, the app would receive the tag, but remain
    // in the background.
    if (m_declarativeView) {
        m_declarativeView->raise();
    }
#endif
    Q_UNUSED(message);
    Q_UNUSED(target);
}


/*!
  \brief Store a pointer to the declarative view.

  Needed for raising the app to the foreground on MeeGo in the
  autostart scenario when the app is already active in the background.
  */
void NfcPeerToPeer::setDeclarativeView(QDeclarativeView& view)
{
    m_declarativeView = &view;
}

bool NfcPeerToPeer::isBusy() const
{
    return m_isBusy;
}
