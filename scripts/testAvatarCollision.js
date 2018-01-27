(function() { // BEGIN LOCAL_SCOPE

	var showNextCollision = false;

	// var joints = MyAvatar.getJointNames();
	// for (var i = joints.length - 1; i >= 0; i--) {
	// 	print(joints[i]);
	// 	// addSphere(MyAvatar.getJointPosition(joints[i]));
	// }
	// addSphere(MyAvatar.getJointPosition("Hips"));
	// addSphere(MyAvatar.position);
function collisionWithEntity(collision) {
	var localPosition =  Vec3.subtract(collision.contactPoint, MyAvatar.position);
	var collisionHeight = localPosition.y;
	var localNeckHeight = Vec3.subtract(MyAvatar.getJointPosition("Neck"), MyAvatar.position).y;
	var localHipHeight = Vec3.subtract(MyAvatar.getJointPosition("Hips"), MyAvatar.position).y;
	var collisionHeightBetweenNeckAndHips = linearMap(collisionHeight, localHipHeight, localNeckHeight, 0.0, 1.0);

	localPosition.y = 0;
	var yAxisRotation = Quat.safeEulerAngles(Quat.rotationBetween(localPosition, Quat.getForward(MyAvatar.orientation))).y;
    // print("collision!: " + JSON.stringify(localPosition));
    print("collision!\nangle: " + yAxisRotation + "\nheight: " + collisionHeightBetweenNeckAndHips);

    VibrateMotorsForCollision(yAxisRotation, collisionHeightBetweenNeckAndHips, 15);
    //TODO: calculate total strength

    // if(showNextCollision) {
    // 	print("showing the above collision!");
    // 	addSphere(collision.contactPoint);
    // 	showNextCollision = false;
    // }
}
MyAvatar.collisionWithEntity.connect(collisionWithEntity);

function addSphere(position) {
 Entities.addEntity({
		position: position,
		type: "Sphere",
		dimensions: {x: .75, y: .01, z: .75},
		name: "collisionLocation",
		color: {red: 20, green: 180, blue: 20},
		parentID: "{00000000-0000-0000-0000-000000000001}",
		collisionMask: 23,// Do not collide with my avatar
		dynamic: true
	});
}

function keyPressEvent(event) {
    if (event.text === 'SPACE') {
        // print("key hit!");
        // strength, duration in seconds, hand
    showNextCollision = true;
    }
}
Controller.keyPressEvent.connect(keyPressEvent);

function linearMap(xIn, inMin, inMax, outMin, outMax) {
	var xOut = (xIn - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
	return xOut;
}

var motors = [
{id: 10, angle: -22.5, height: 0}, 
{id: 9, angle: -67.5, height: 0}, 
{id: 7, angle: -112.5, height: 0}, 
{id: 8, angle: -157.5, height: 0}, 
{id: 21, angle: 157.5, height: 0}, 
{id: 20, angle: 112.5, height: 0}, 
{id: 22, angle: 67.5, height: 0}, 
{id: 19, angle: 22.5, height: 0}, 
{id: 5, angle: -22.5, height: 0.25}, 
{id: 3, angle: -67.5, height: 0.25}, 
{id: 18, angle: -112.5, height: 0.25}, 
{id: 16, angle: -157.5, height: 0.25}, 
{id: 29, angle: 157.5, height: 0.25}, 
{id: 27, angle: 112.5, height: 0.25}, 
{id: 24, angle: 67.5, height: 0.25}, 
{id: 26, angle: 22.5, height: 0.25}, 
{id: 6, angle: -22.5, height: 0.75}, 
{id: 4, angle: -67.5, height: 0.75}, 
{id: 17, angle: -112.5, height: 0.75}, 
{id: 15, angle: -157.5, height: 0.75}, 
{id: 28, angle: 157.5, height: 0.75}, 
{id: 30, angle: 112.5, height: 0.75}, 
{id: 25, angle: 67.5, height: 0.75}, 
{id: 23, angle: 22.5, height: 0.75}, 
{id: 14, angle: -22.5, height: 1}, 
{id: 13, angle: -67.5, height: 1}, 
{id: 11, angle: -112.5, height: 1}, 
{id: 12, angle: -157.5, height: 1}, 
{id: 0, angle: 157.5, height: 1}, 
{id: 1, angle: 112.5, height: 1}, 
{id: 2, angle: 67.5, height: 1}, 
{id: 31, angle: 22.5, height: 1}
]

// Scales: angle (-180, 180], height [0,1] (for the torso, although could be outside of that range), totalStrength [0,15]
function VibrateMotorsForCollision(angle, height, totalStrength) {
	var MAXIMUM_DISTANCE = .5;
	for (var i = motors.length - 1; i >= 0; i--) {
		motor = motors[i];
		print(motor.id);
		var verticalDistance = height - motor.height;
		print(verticalDistance);
		//Find shortest rotation
		var angularDistance = angle - motor.angle;
		angularDistance = angularDistance > 180 ? 360 - angularDistance : angularDistance;
		angularDistance = angularDistance < -180 ? 360 + angularDistance : angularDistance;
		angularDistance = linearMap(angularDistance, -180, 180, 0, 2);
		print(angularDistance);
		var combinedDistance = Math.sqrt(verticalDistance * verticalDistance + angularDistance * angularDistance);
		print(combinedDistance);
		if(combinedDistance < MAXIMUM_DISTANCE) {
			var strengthForThisMotor = linearMap(combinedDistance, 0, MAXIMUM_DISTANCE, totalStrength, 0);
			// Controller.triggerHapticPulse(strengthForThisMotor, 0.1, motor.id);
			print("Triggering motor " + motor.id + " at strength " + strengthForThisMotor + " for collision at angle " + angle + " and height " + height);
		}
	}
}

// Script.scriptEnding.connect(cleanup);
}()); // END LOCAL_SCOPE