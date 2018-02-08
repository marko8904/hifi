var motors = [
{id: 10, x: -0.042, y: 0, z: 0.101}, 
{id: 9, x: -0.101, y: 0, z: 0.042}, 
{id: 7, x: -0.101, y: 0, z: -0.042}, 
{id: 8, x: -0.042, y: 0, z: -0.101}, 
{id: 21, x: 0.042, y: 0, z: -0.101}, 
{id: 20, x: 0.101, y: 0, z: -0.042}, 
{id: 22, x: 0.101, y: 0, z: 0.042}, 
{id: 19, x: 0.042, y: 0, z: 0.101}, 
{id: 5, x: -0.042, y: 0.13, z: 0.101}, 
{id: 3, x: -0.101, y: 0.13, z: 0.042}, 
{id: 18, x: -0.101, y: 0.13, z: -0.042}, 
{id: 16, x: -0.042, y: 0.13, z: -0.101}, 
{id: 29, x: 0.042, y: 0.13, z: -0.101}, 
{id: 27, x: 0.101, y: 0.13, z: -0.042}, 
{id: 24, x: 0.101, y: 0.13, z: 0.042}, 
{id: 26, x: 0.042, y: 0.13, z: 0.101}, 
{id: 6, x: -0.042, y: 0.26, z: 0.101}, 
{id: 4, x: -0.101, y: 0.26, z: 0.042}, 
{id: 17, x: -0.101, y: 0.26, z: -0.042}, 
{id: 15, x: -0.042, y: 0.26, z: -0.101}, 
{id: 28, x: 0.042, y: 0.26, z: -0.101}, 
{id: 30, x: 0.101, y: 0.26, z: -0.042}, 
{id: 25, x: 0.101, y: 0.26, z: 0.042}, 
{id: 23, x: 0.042, y: 0.26, z: 0.101}, 
{id: 14, x: -0.042, y: 0.4, z: 0.101}, 
{id: 13, x: -0.101, y: 0.4, z: 0.042}, 
{id: 11, x: -0.101, y: 0.4, z: -0.042}, 
{id: 12, x: -0.042, y: 0.4, z: -0.101}, 
{id: 0, x: 0.042, y: 0.4, z: -0.101}, 
{id: 1, x: 0.101, y: 0.4, z: -0.042}, 
{id: 2, x: 0.101, y: 0.4, z: 0.042}, 
{id: 31, x: 0.042, y: 0.4, z: 0.101}
];

// for (var i = motors.length - 1; i >= 0; i--) {
// 	motor = motors[i];
// 	var entityID = Entities.addEntity({
// 		position: getVectorRelativeToAvatar(motor.x, motor.y, motor.z),
// 		type: "Box",
// 		dimensions: {x: .05, y: .05, z: .05},
// 		name: "VestBox" + motor.id,
// 		color: {red: motor.id, green: 1, blue: 0},
// 		parentID: "{00000000-0000-0000-0000-000000000001}",
// 		script: Script.resolvePath("vestCollisionBox.js"),
// 		collisionMask: 23,// Do not collide with my avatar
// 		dynamic: true,
// 		visible: true,
// 		parentJointIndex: 16,
// 		grabbable: false
// 	}, true);
// }


var update = function(delta){
	// var avatars = AvatarList.getAvatarIdentifiers();
	// avatars.forEach(function(id){
	// 	var avatar = AvatarList.getAvatar(id);
	// 	["RightHandPinky2","LeftHandPinky2"].forEach(function(joint){
	// 		var jointPos = avatar.getJointPosition(joint);
	// 		var distance = Vec3.distance(Entities.getEntityProperties(_this.entityID).position, jointPos);
	// 		if(distance < .1){
	// 			// print("motor " + _this.motorID + " is distance " + distance + " from joint " + joint);
	// 			Controller.triggerHapticPulse(15, .25, _this.motorID);
	// 			setColor(_this.entityID, distance * 20)
	// 		}
	// 	});
	// });
	print("updating!: " + delta);
}

Script.update.connect(update);

function getVectorRelativeToAvatar(x,y,z){
	var frontVector = Vec3.multiply(Quat.getFront(MyAvatar.orientation), z);
	var rightVector = Vec3.multiply(Quat.getRight(MyAvatar.orientation), x);
	var upVector = Vec3.multiply(Quat.getUp(MyAvatar.orientation), y);
	var combinedVectors = Vec3.sum(frontVector, Vec3.sum(rightVector, upVector));
	return Vec3.sum(combinedVectors, MyAvatar.position);
}
