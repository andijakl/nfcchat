Nfc Chat
========
The Nfc Chat is a sample app that demonstrates how to use the NFC LLCP protocol to create a peer-to-peer connection between two compatible NFC Forum devices and how to send messages between those devices. 

A custom-made user interface using Qt Quick Components uses a model to store the chat- and status messages and a simple listview to show them on the screen. The C++ class NfcPeerToPeer takes care of listening for NFC targets and establishing a LLCP socket connection to the client. A LLCP server is waiting for incoming connections at a specified URI and then provides a socket to send data to the client.

The C++ class is used from within the QML code and can be easily controlled - sending text to the other device is done using a slot. Both received messages and status messages are emitted using signals. In case the connection to the second device is lost, the application caches the message and sends it as soon as the link is re-established.

Note for Symbian: The project requires Qt 4.7.4, Qt Mobility 1.2 and Qt Quick Components to be present on your device. The binary does not use the Smart Installer at the moment, so you need to manually install dependencies if needed. On the C7, Symbian Anna (or newer) is required to use this application, as it enables the NFC capabilities of the device. Please read the build & installation instructions for more details.

More information:
http://www.nfcinteractor.com/apps/nfcchat/

Known bugs / issues (work-in-progress):
- Complete source code comments


IMPORTANT FILES/CLASSES
-------------------------------------------------------------------------------

- nfcpeertopeer.h : encapsulates NFC target detection and both the server socket and client socket handling.
- main.qml : main window for the application
- NfcChatPage.qml : chat page, defines the model containing the chat messages, the list view and the edit element plus a simple button to send messages.


SECURITY
--------------------------------------------------------------------------------

Symbian: The application can be self-signed.

Harmattan: No special aegis manifest is required.


KNOWN ISSUES
-------------------------------------------------------------------------------

None


BUILD & INSTALLATION INSTRUCTIONS
-------------------------------------------------------------------------------

MeeGo Harmattan
~~~~~~~~~~~~~~~
The example will work out of the box with the Harmattan target 
of the Qt SDK 1.1.4. Make sure you have the latest firmware version
on your Nokia N9. The Nokia N950 doesn't support NFC.


Symbian
~~~~~~~
Compatible devices:
C7-00/Oro/Astound with Symbian Anna/Belle,
Nokia 603, 700 and 701 (plus all upcoming NFC capable devices).

Device Preparation:
C7-00 with Symbian Anna: additionally install Qt 4.7.4, Qt Mobility 1.2 
and Qt Quick Components 1.1 from the Qt SDK:
C:\QtSDK\Symbian\sis\Symbian_Anna\Qt\4.7.4\Qt-4.7.403-for-Anna.sis
C:\QtSDK\Symbian\sis\Symbian_Anna\Qt\4.7.4\QtWebKit-4.8.1-for-Anna.sis
C:\QtSDK\Symbian\sis\Symbian_Anna\QtMobility\1.2.1\QtMobility-1.2.1-for-Anna.sis
C:\QtSDK\Symbian\sis\Symbian_Anna\QtQuickComponents\1.1\QtQuickComponents-1.1-for-Anna-Belle.sis

Symbian Belle: already includes Qt 4.7.4 and QtM 1.2 - you don't
need to install these. Only Qt Quick Components is required:
C:\QtSDK\Symbian\sis\Symbian_Anna\QtQuickComponents\1.1\QtQuickComponents-1.1-for-Anna-Belle.sis



Build & installation instructions using Qt SDK
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Open nfcchat.pro
   File > Open File or Project, select nfcchat.pro.
   
2. Symbian: Select the 'Qt 4.7.4 for Symbian Anna" target
   (compatible to phones with Symbian Belle).
   MeeGo Harmattan: Select the MeeGo 1.2 Harmattan (Qt SDK) release target.

3. Press the Run button to build the project and to install it on the device.

Note: if switching between Symbian and MeeGo Harmattan builds, make sure to 
clean the project inbetween. Otherwise, specific differences in the meta-objects
might not get rebuilt.


COMPATIBILITY
-------------------------------------------------------------------------------

- Qt SDK 1.2 / Qt Creator 2.4
- QtMobility 1.2.x
- Qt 4.7.4
- Qt Quick Components 1.1

Tested on: 
- Nokia C7-00 with Symbian Anna Firmware and QtM 1.2
- Nokia C7-00 and 701 with Symbian Belle Firmware (already includes QtM 1.2)
- Nokia N9 PR 1.1 / PR 1.2


CHANGE HISTORY
--------------------------------------------------------------------------------

2.0.0 (work in progress)
      Connectionless sockets.
	  Connect only server- or client socket in connection oriented mode.
1.3.1 Update of release notes and recompiled with latest Qt Mobility 1.2.
1.3.0 Corrected LLCP URI according to NFC standards. 
      Text wrapping in the view.
1.2.0 Option to choose whether to send outgoing messages through the server or the
      client socket.
1.1.0 Ported UI to Qt Quick Components.
  	  Added MeeGo Harmattan support.
      Symbian: added NFC availability check, incorporating the NFC Settings App:
	           https://projects.developer.nokia.com/NfcSettingsApplication/
1.0.0 First version.


RELATED DOCUMENTATION
-------------------------------------------------------------------------------

Project page
http://www.nfcinteractor.com/apps/nfcchat/

Related example projects
http://www.nfcinteractor.com/

NFC 
http://www.developer.nokia.com/NFC

Qt
http://www.developer.nokia.com/Qt

