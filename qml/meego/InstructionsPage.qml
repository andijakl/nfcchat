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
            onClicked: pageStack.depth <= 1 ? Qt.quit() : pageStack.pop()
        }
    }

    Item {
        id: instructionsItem
        anchors.fill: parent
        anchors.leftMargin: customPlatformStyle.paddingLarge
        anchors.rightMargin: customPlatformStyle.paddingLarge

        Flickable {
            id: instructionsFlickable
            width: parent.width
            height: parent.height
            anchors { left: parent.left; top: parent.top }
            contentHeight: instructionsContentItem.height
            flickableDirection: Flickable.VerticalFlick

            Column {
                id: instructionsContentItem
                width: parent.width

                spacing: customPlatformStyle.paddingLarge

                Text {
                    id: instructionsText1
                    text: qsTr("<strong>Nfc Chat</strong>")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    //color: customPlatformStyle.colorNormalLight
                    color: "aliceblue";
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeLarge
                }
                Image {
                    id: instructionsImage
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                    source: "icon.png"
                    asynchronous: true
                }

                Text {
                    id: instructionsText2
                    text: qsTr("v2.0.0\n2011 - 2012 Andreas Jakl")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: customPlatformStyle.colorNormalLight
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }
                Button {
                    id: instructionsLinkButton
                    text: "NfcInteractor.com"
                    onClicked: Qt.openUrlExternally("http://www.nfcinteractor.com/nfcchat/");
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: instructionsText3
                    text: qsTr("Exchange chat messages between NFC devices using LLCP.\n\nStart the application on two compatible, NFC-enabled Symbian phones or the Nokia N9. Ensure the connection settings match on both phones. Then, touch the two phones at the position of the NFC antenna and keep them close together. Enter a message in the text box at the top of the chat page and press the send button to transmit the message to the other phone via NFC.")
                    horizontalAlignment: Text.AlignHCenter
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: customPlatformStyle.colorNormalLight
                    font.family: customPlatformStyle.fontFamilyRegular;
                    font.pixelSize: customPlatformStyle.fontSizeMedium
                }
            }

        }

        ScrollDecorator {
            flickableItem: instructionsFlickable
        }

    }
}
