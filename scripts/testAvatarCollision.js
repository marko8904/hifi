(function() { // BEGIN LOCAL_SCOPE

function collisionWithEntity(collision) {
    print("collision!");
}
MyAvatar.collisionWithEntity.connect(collisionWithEntity);

// Script.scriptEnding.connect(cleanup);
}()); // END LOCAL_SCOPE