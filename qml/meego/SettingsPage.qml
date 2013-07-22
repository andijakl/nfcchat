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
import com.nokia.meego 1.0

Page {
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: {
                saveSettings();
                pageStack.depth <= 1 ? Qt.quit() : pageStack.pop();
            }

        }
    }

    // Settings
    property alias useConnectionLess: connectionSwitch.checked
    property alias nfcPort: clPort.text
    property alias nfcUri: coUri.text
    property alias sendThroughServerSocket: coSendSocketSwitch.checked
    property alias connectClientSocket: coClientSocket.checked
    property alias connectServerSocket: coServerSocket.checked

    // Internal use - for virtual keyboard handling
    property Item focusedItem

    Row {
        id: settingsHeader
        spacing: customPlatformStyle.paddingMedium;
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Image {
            id: settingsImage
            source: "settings.svg"
            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }

        Text {
            id: settingsHeaderText
            text: qsTr("Settings")
            verticalAlignment: Text.AlignVCenter
            height: settingsImage.height
            font.family: customPlatformStyle.fontFamilyRegular;
            color: customPlatformStyle.colorNormalLight
            font.pixelSize: customPlatformStyle.fontSizeLarge
        }
    }

    Rectangle {
        id: separator
        width: parent.width; height: 1; color: "gray"
        anchors.top: settingsHeader.bottom
    }

    Flickable {
        id: flickSettings

        clip: true
        contentHeight: connectionMode.height + customPlatformStyle.paddingMedium * 3 + connectionRow.height + (connectionSwitch.checked ? clColumn.height : coColumn.height);
        anchors.top: separator.bottom
        anchors.topMargin: customPlatformStyle.paddingMedium;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Text {
            id: connectionMode
            text: qsTr("Connection Mode")
            font.family: customPlatformStyle.fontFamilyRegular;
            color: customPlatformStyle.colorNormalLight
            font.pixelSize: customPlatformStyle.fontSizeMedium
        }

        // Switch: Connection oriented / less
        Row {
             id: connectionRow
             anchors.top: connectionMode.bottom
             anchors.topMargin: customPlatformStyle.paddingMedium;
             height: connectionSwitch.height
             width: parent.width
             spacing: customPlatformStyle.paddingMedium;

             Switch {
                 id: connectionSwitch
             }

             Text {
                 width: connectionRow.width - connectionRow.spacing - connectionSwitch.width
                 height: connectionSwitch.height
                 verticalAlignment: Text.AlignVCenter
                 text: connectionSwitch.checked ? "Connection-less" : "Connection-oriented";
                 font.family: customPlatformStyle.fontFamilyRegular;
                 color: customPlatformStyle.colorNormalLight
                 font.pixelSize: customPlatformStyle.fontSizeMedium
             }
         }

        // --------------------------------------------------------------------------------
        // Connection-less
        Column {
            id: clColumn
            visible: connectionSwitch.checked
            spacing: customPlatformStyle.paddingMedium;
            width: parent.width
            anchors.top: connectionRow.bottom
            anchors.topMargin: customPlatformStyle.paddingMedium;

            // - Port
            Text {
                text: qsTr("Port")
                font.family: customPlatformStyle.fontFamilyRegular;
                color: customPlatformStyle.colorNormalLight
                font.pixelSize: customPlatformStyle.fontSizeMedium
            }

            TextField {
                id: clPort
                width: parent.width
                text: "35"
                validator: IntValidator{bottom: 1; top: 65536}
                onActiveFocusChanged: if (activeFocus) focusedItem = clPort
            }
        }

        // --------------------------------------------------------------------------------
        // Connection-oriented
        Column {
            id: coColumn
            visible: !connectionSwitch.checked
            spacing: customPlatformStyle.paddingMedium;
            width: parent.width
            anchors.top: connectionRow.bottom
            anchors.topMargin: customPlatformStyle.paddingMedium;

            // - URI
            Text {
                text: qsTr("URI")
                font.family: customPlatformStyle.fontFamilyRegular;
                color: customPlatformStyle.colorNormalLight
                font.pixelSize: customPlatformStyle.fontSizeMedium
            }

            TextField {
                id: coUri
                width: parent.width
                text: "urn:nfc:xsn:nokia.com:nfcchat"
                maximumLength: 255
                onActiveFocusChanged: if (activeFocus) focusedItem = coUri
            }

            // - Connect Client Socket
            CheckBox {
                id: coClientSocket
                checked: true
                text: "Connect Client Socket"
                onClicked: {
                    // Do not allow disabling both sockets
                    if (!coClientSocket.checked &&
                            !coServerSocket.checked) {
                        coServerSocket.checked = true;
                    }
                    // Make sure sending is done through a connected socket
                    checkSendSocketSwitch();
                }
            }

            // - Connect Server Socket
            CheckBox {
                id: coServerSocket
                checked: true
                text: "Connect Server Socket"
                onClicked: {
                    // Do not allow disabling both sockets
                    if (!coServerSocket.checked &&
                            !coClientSocket.checked) {
                        coClientSocket.checked = true;
                    }
                    // Make sure sending is done through a connected socket
                    checkSendSocketSwitch();
                }
            }


            // - Send messages through: server or client socket
            // (only if connect on this socket is enabled)
            Text {
                text: qsTr("Send messages through")
                font.family: customPlatformStyle.fontFamilyRegular;
                color: customPlatformStyle.colorNormalLight
                font.pixelSize: customPlatformStyle.fontSizeMedium
            }

            Row {
                id: coSendSocketRow
                spacing: customPlatformStyle.paddingMedium;
                width: parent.width
                height: coSendSocketSwitch.height

                // Checked: server socket
                // Unchecked: client socket
                Switch {
                    id: coSendSocketSwitch
                }

                Text {
                    width: coSendSocketRow.width - coSendSocketRow.spacing - coSendSocketSwitch.width
                    height: coSendSocketSwitch.height
                    verticalAlignment: Text.AlignVCenter
                    text: coSendSocketSwitch.checked ? "Server socket" : "Client socket"
                    font.family: customPlatformStyle.fontFamilyRegular;
                    color: customPlatformStyle.colorNormalLight
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }
            }
        }
    }


    function checkSendSocketSwitch() {
        if (!coClientSocket.checked &&
                !coSendSocketSwitch.checked) {
            // Client socket isn't connected, but the switch
            // instructs to use the client socket to send messages
            // -> change to use the server socket
            coSendSocketSwitch.checked = true;
        }
        if (!coServerSocket.checked &&
                coSendSocketSwitch.checked) {
            // Server socket isn't connected, but the switch
            // instructs to use the server socket to send messages
            // -> change to use the client socket
            coSendSocketSwitch.checked = false;
        }

        // Enable send socket choose switch only if both sockets
        // are connected
        coSendSocketSwitch.enabled = (coServerSocket.checked &&
                                      coClientSocket.checked);

    }


    // Note: VKB-Tricks with inputContext not needed on MeeGo
}
