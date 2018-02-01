(function(){
	function VestCollisionBox(){
		return;
	}

	var _this;
	var motorID;

	VestCollisionBox.prototype = {
		preload: function(entityID){
			_this = this;
			_this.entityID = entityID;
			_this.motorID = getMotorID();
			// print('created collision box with motorID: ' + getMotorID());
		},
		collisionWithEntity: function(myID, otherID, collisionInfo){
			changeColor(myID);
			Controller.triggerHapticPulse(15,.25, MotorID);
		}
	}

	var changeColor = function(entityID){
			var red = getMotorID();
			var green = Math.random()*255;
			var blue = Math.random()*255;
			Entities.editEntity(entityID, {color:{red: red, green: green, blue: blue}});
	}

	var getMotorID = function(){
		return Entities.getEntityProperties(_this.entityID).color.red;
	}

	return new VestCollisionBox();
});
