package benkuper.nativeExtensions

{
	import flash.events.EventDispatcher;
	import flash.geom.Vector3D;
	
	/**
	 * ...
	 * @author Ben Kuper
	 */
	public class Myo extends EventDispatcher 
	{
		//Pose names according to SDK documentation so there is no switch needed
		static public const POSE_NONE:String = "rest";
		static public const POSE_FIST:String = "fist";
		static public const POSE_WAVE_IN:String = "waveIn";
		static public const POSE_WAVE_OUT:String = "waveOut";
		static public const POSE_FINGERS_SPREAD:String = "fingersSpread";
		static public const POSE_TWIST_IN:String = "thumbToPinky";
		private var _pose:String;
		
		static public const VIBRATION_SHORT:int = 0;
		static public const VIBRATION_MEDIUM:int = 1;
		static public const VIBRATION_LONG:int = 2;
		
		private var _id:String;
		private var _connected:Boolean;
		public var isTrained:Boolean;
		
		public var yaw:Number;
		public var pitch:Number;
		public var roll:Number;
		
		public var accel:Vector.<Number>;
		public var gyro:Vector.<Number>;
		
		public function Myo(id:String):void
		{
			this._id = id;
			accel = new Vector.<Number>;
			gyro = new Vector.<Number>;
			_pose = POSE_NONE;
			for (var i:int = 0; i < 3; i++)
			{
				accel.push(0);
				gyro.push(0);
			}
		}
		
		public function updateData(connected:Boolean,pose:String, y:Number,p:Number,r:Number, ax:Number,ay:Number,az:Number,gx:Number,gy:Number,gz:Number):void
		{
			this.connected = connected;
			this.pose = pose;
			
			yaw = y;
			pitch = p;
			roll = r;
			
			accel[0] = ax;
			accel[1] = ay;
			accel[2] = az;
			gyro[0] = gx;
			gyro[1] = gy;
			gyro[2] = gz;
			
			dispatchEvent(new MyoEvent(MyoEvent.ORIENTATION_UPDATE,this));  
		}
		
		public function vibrate(vibrationType:int = VIBRATION_SHORT):void
		{
			MyoController.instance.vibrate(this.id, vibrationType);
		}
		
		
		//Getter / setter
		
		public function get pose():String 
		{
			return _pose;
		}
		
		public function set pose(value:String):void 
		{
			if (pose == value) return;
			_pose = value;
			dispatchEvent(new MyoEvent(MyoEvent.POSE_UPDATE,this));
		}	
		
		public function get connected():Boolean 
		{
			return _connected;
		}
		
		public function set connected(value:Boolean):void  
		{
			if (connected == value) return;
			_connected = value;
			
			dispatchEvent(new MyoEvent(value?MyoEvent.MYO_CONNECTED:MyoEvent.MYO_DISCONNECTED,this));
		}
		
		public function get id():String 
		{
			return _id;
		}
		
		override public function toString():String
		{
			return "[Myo id(macAddress)=" + id + ", connected="+connected+", yaw/pitch/roll=" + yaw + "/" + pitch + "/" + roll + ", pose=" + pose + "]";
		}
		
		
	}
	
}