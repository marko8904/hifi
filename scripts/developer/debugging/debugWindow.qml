//
//  InfoView.qml
//
//  Created by Bradley Austin Davis on 27 Apr 2015
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.5
import QtQuick.Controls 1.4
import Hifi 1.0 as Hifi


Rectangle {
    id: root
    width: parent ? parent.width : 100
    height: parent ? parent.height : 100

    property var channel;

    TextArea {
        id: textArea
        width: parent.width
        height: parent.height
        text:""
    }

    function fromScript(message) {
        textArea.text += message + "\n";
    }
}


