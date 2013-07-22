/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Andreas Jakl (andreas.jakl@mopius.com)
**
** This file is part of an NFC example for Qt Mobility.
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

import QtQuick 1.1
import com.nokia.symbian 1.1 // Symbian Qt Quick components

PageStackWindow {
    id: window
    initialPage: nfcChatPage
    showStatusBar: true
    showToolBar: true

    // The Nfc Chat page is the main page
    NfcChatPage {
        id: nfcChatPage
    }

    function applySettingsToPage() {
        settingsLoader.item.useConnectionLess = settings.useConnectionLess;
        settingsLoader.item.nfcPort = settings.nfcPort;
        settingsLoader.item.nfcUri = settings.nfcUri;
        settingsLoader.item.sendThroughServerSocket = settings.sendThroughServerSocket;
        settingsLoader.item.connectClientSocket = settings.connectClientSocket;
        settingsLoader.item.connectServerSocket = settings.connectServerSocket;
    }

    function saveSettings() {
        // Apply the new settings to the nfcPeerToPeer object
        settings.setUseConnectionLess(settingsLoader.item.useConnectionLess);
        settings.setNfcPort(settingsLoader.item.nfcPort);
        settings.setNfcUri(settingsLoader.item.nfcUri);
        settings.setSendThroughServerSocket(settingsLoader.item.sendThroughServerSocket);
        settings.setConnectClientSocket(settingsLoader.item.connectClientSocket);
        settings.setConnectServerSocket(settingsLoader.item.connectServerSocket);
        nfcChatPage.applyNfcSettings();
    }

    // Asynchronously load the settings and the instructions page
    Loader {
        id: instructionsLoader
        anchors.fill: parent
    }
    Loader {
        id: settingsLoader
        anchors.fill: parent
        onStatusChanged:
            if (status === Loader.Ready) {
                // Load the settings from the database
                applySettingsToPage();
            }
    }

    Component.onCompleted: {
        // Start loading the sub-pages (instructions & settings)
        timer.restart();
    }

    Timer {
        id: timer
        interval: 50
        repeat: false
        onTriggered: {
            instructionsLoader.source = Qt.resolvedUrl("InstructionsPage.qml");
            settingsLoader.source = Qt.resolvedUrl("SettingsPage.qml");
        }
    }


}
