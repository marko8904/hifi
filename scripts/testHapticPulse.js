(function() { // BEGIN LOCAL_SCOPE

function keyPressEvent(event) {
    if (event.text === 'SPACE') {
        print("key hit!");
        // strength, duration in seconds, hand
        Controller.triggerHapticPulse(15, 1.0, 3);
    }
}
Controller.keyPressEvent.connect(keyPressEvent);

// Script.scriptEnding.connect(cleanup);
}()); // END LOCAL_SCOPE