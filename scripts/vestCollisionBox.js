(function(){
	function VestCollisionBox(){
		return;
	}

	var _this;
	var motorID;

	// var jointsToCheck = ["RightArm","RightForeArm","RightHand","RightHandPinky1","RightHandPinky2","RightHandPinky3","RightHandPinky4","RightHandRing1","RightHandRing2","RightHandRing3","RightHandRing4","RightHandIndex1","RightHandIndex2","RightHandIndex3","RightHandIndex4","RightHandThumb1","RightHandThumb2","RightHandThumb3","RightHandThumb4","RightHandMiddle1","RightHandMiddle2","RightHandMiddle3","RightHandMiddle4","LeftArm","LeftForeArm","LeftHand","LeftHandPinky1","LeftHandPinky2","LeftHandPinky3","LeftHandPinky4","LeftHandRing1","LeftHandRing2","LeftHandRing3","LeftHandRing4","LeftHandIndex1","LeftHandIndex2","LeftHandIndex3","LeftHandIndex4","LeftHandThumb1","LeftHandThumb2","LeftHandThumb3","LeftHandThumb4","LeftHandMiddle1","LeftHandMiddle2","LeftHandMiddle3","LeftHandMiddle4"];

	var update = function(delta){
		var avatars = AvatarList.getAvatarIdentifiers();
		avatars.forEach(function(id){
			var avatar = AvatarList.getAvatar(id);
			["RightHandPinky2","LeftHandPinky2"].forEach(function(joint){
				var jointPos = avatar.getJointPosition(joint);
				var distance = Vec3.distance(Entities.getEntityProperties(_this.entityID).position, jointPos);
				if(distance < .1){
					// print("motor " + _this.motorID + " is distance " + distance + " from joint " + joint);
					Controller.triggerHapticPulse(15, .25, _this.motorID);
					setColor(_this.entityID, distance * 20)
				}
			});
		});
	}

	Script.update.connect(update);

	VestCollisionBox.prototype = {
		preload: function(entityID){
			_this = this;
			_this.entityID = entityID;
			_this.motorID = getMotorID();
//			print('created collision box with motorID: ' + _this.motorID);
		},
		collisionWithEntity: function(myID, otherID, collisionInfo){
			changeColor(myID);
			Controller.triggerHapticPulse(15,.25, _this.motorID);
		}
	}

	var changeColor = function(entityID){
			var red = _this.motorID;
			var green = Math.random()*255;
			var blue = Math.random()*255;
			Entities.editEntity(entityID, {color:{red: red, green: green, blue: blue}});
	}

	var setColor = function(entityID, value){
		var red = _this.motorID;
		var green = value*255;
		var blue = value*255;
		Entities.editEntity(entityID, {color:{red: red, green: green, blue: blue}});
	}

	var getMotorID = function(){
		return Entities.getEntityProperties(_this.entityID).color.red;
	}

	return new VestCollisionBox();
});
