(function(){
	function VestCollisionBox(){
		return;
	}

	var _this;

	VestCollisionBox.prototype = {
		preload: function(entityID){
			_this = this;
			_this.entityID = entityID;
			print('created collision box with motorID: ' + getMotorID());
		},
		clickDownOnEntity: function(entityID, event){
			// print("properties: " + JSON.stringify(Entities.getEntityProperties(entityID)));
			changeColor(entityID);
			print('Motor ID: ' + getMotorID());
			Controller.triggerHapticPulse(15,3.5, getMotorID());
		},
		collisionWithEntity: function(myID, otherID, collisionInfo){
			// print("collision!: " + Entities.getEntityProperties(myID).motorID);
			changeColor(myID);
			changeColor(otherID);
			Controller.triggerHapticPulse(15,.25, getMotorID());
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
