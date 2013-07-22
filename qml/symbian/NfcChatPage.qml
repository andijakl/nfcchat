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

import NfcPeerToPeer 1.0

Page {
    Connections {
        target: nfcPeerToPeer
        onChatMessage: {
            console.log("Chat message: " + nfcClientMessage);
            logMessage(nfcClientMessage, "aquamarine", "nfcSymbolMsg.png");
        }
        onStatusMessage: {
            console.log("Status message: " + statusMessage);
            logMessage(statusMessage, "aliceblue", "nfcSymbolInfo.png");
        }
        onNfcStatusError: {
            console.log("Nfc status error: " + nfcStatusErrorText);
            logMessage(nfcStatusErrorText, "coral", "nfcSymbolError.png");
        }
        onNfcStatusUpdate: {
            console.log("Nfc status update: " + nfcStatusText);
            logMessage(nfcStatusText, "aliceblue", "nfcSymbolInfo.png");
        }
        onSettingsApplied: {
            logMessage("New settings applied", "aliceblue", "nfcSymbolSuccess.png");
        }
    }


    // Add a message to the model
    function logMessage(text, color, img)
    {
        messageModel.append( {"infoMsg": text, "textColor": color, "image": img} )
    }

    Timer {
        id: timerInitNfc
        interval: 10
        repeat: false
        onTriggered: {
            initNfc();
        }
    }
    function initNfc()
    {
        nfcPeerToPeer.checkNfcStatus();
        nfcPeerToPeer.initAndStartNfc();
    }

    function applyNfcSettings() {
        logMessage("Applying settings ...", "aliceblue", "nfcSymbolInfo.png");
        nfcPeerToPeer.applySettings();
    }

    ListModel {
        id: messageModel
    }

    Button {
        id: sendButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: customPlatformStyle.paddingMedium
        width: 80
        iconSource: "nfcSendButtonWhite.png" //+ (pressed ? "Black" : "White") + ".png"
        onClicked: nfcPeerToPeer.sendText(sendTextEdit.text)
    }

    TextField {
        id: sendTextEdit
        text: "Hello NFC"
        anchors {top: parent.top; left: parent.left; right:sendButton.left }
        anchors.topMargin: customPlatformStyle.paddingMedium
    }

    ListView {
        id: messageView
        model: messageModel
        clip: true
        delegate: listDelegate
        anchors { top: sendButton.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
        anchors.margins: customPlatformStyle.paddingMedium
        // Automatically scroll down when a new element is added
        onCountChanged: positionViewAtEnd();
        Component.onCompleted: positionViewAtBeginning();
    }


    // Delegate for showing an individual line of the model
    Component {
        id: listDelegate
        Item {
            width: parent.width
            height: Math.max(infoImg.height, infoTxt.paintedHeight) + customPlatformStyle.paddingMedium
            Item {
                id: row
                width: parent.width
                Image {
                    id: infoImg
                    source: image
                    fillMode: Image.PreserveAspectFit
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.topMargin: Math.ceil((customPlatformStyle.fontHeightMedium - infoImg.sourceSize.height) / 2)
                    asynchronous: true
                }
                Text {
                    id: infoTxt
                    text: infoMsg
                    color: textColor
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                    wrapMode: Text.Wrap
                    anchors.left: infoImg.right
                    anchors.leftMargin: customPlatformStyle.paddingMedium
                    anchors.right: parent.right
                    anchors.rightMargin: customPlatformStyle.paddingSmall
                }
            }
        }
    }

    // -------------------------------------------------------------------------
    // Working indicator
    BusyIndicator {
        id: busySpinner
        visible: nfcPeerToPeer.isBusy
        running: nfcPeerToPeer.isBusy
        width: 50
        height: 50
        anchors.bottom: parent.bottom
        anchors.bottomMargin: customPlatformStyle.paddingMedium
        anchors.right: parent.right
        anchors.rightMargin: customPlatformStyle.paddingMedium
    }

    // -------------------------------------------------------------------------
    // Toolbar
    tools: ToolBarLayout {
        ToolButton {
            flat: true
            iconSource: "toolbar-back";
            onClicked: pageStack.depth <= 1 ? Qt.quit() : pageStack.pop()
        }
        ToolButton {
            flat: true
            iconSource: "info.svg";
            visible: instructionsLoader.status === Loader.Ready
            onClicked: {
                pageStack.push(instructionsLoader.item)
            }
        }
        ToolButton {
            flat: true
            iconSource: "toolbar-settings";
            visible: settingsLoader.status === Loader.Ready
            onClicked: {
                pageStack.push(settingsLoader.item)
            }
        }
    }

    Component.onCompleted: timerInitNfc.restart();
}
