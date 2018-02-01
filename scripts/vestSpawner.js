var cubes = [
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

for (var i = cubes.length - 1; i >= 0; i--) {
	cube = cubes[i];
	var entityID = Entities.addEntity({
		position: getVectorRelativeToAvatar(cube.x, cube.y, cube.z),
		type: "Box",
		dimensions: {x: .05, y: .05, z: .05},
		name: "VestBox" + cube.id,
		color: {red: cube.id, green: 1, blue: 0},
		parentID: "{00000000-0000-0000-0000-000000000001}",
		script: Script.resolvePath("vestCollisionBox.js"),
		collisionMask: 23,// Do not collide with my avatar
		dynamic: true,
		visible: true
	});
}

function getVectorRelativeToAvatar(x,y,z){
	var frontVector = Vec3.multiply(Quat.getFront(MyAvatar.orientation), z);
	var rightVector = Vec3.multiply(Quat.getRight(MyAvatar.orientation), x);
	var upVector = Vec3.multiply(Quat.getUp(MyAvatar.orientation), y);
	var combinedVectors = Vec3.sum(frontVector, Vec3.sum(rightVector, upVector));
	return Vec3.sum(combinedVectors, MyAvatar.position);
}
