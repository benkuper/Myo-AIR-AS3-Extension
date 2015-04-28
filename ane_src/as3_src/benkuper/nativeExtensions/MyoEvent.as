package  benkuper.nativeExtensions
{
	import flash.events.Event;
	
	/**
	 * ...
	 * @author Ben Kuper
	 */
	public class MyoEvent extends Event 
	{
		static public const MYO_PAIRED:String = "myoPaired";
		static public const MYO_CONNECTED:String = "myoConnected";
		static public const MYO_DISCONNECTED:String = "myoDisconnected";
		
		static public const ERROR:String = "error";
		
		static public const STATUS_UPDATE:String = "statusUpdate";
		static public const ORIENTATION_UPDATE:String = "orientationUpdate";
		static public const POSE_UPDATE:String = "poseUpdate";
		static public const EMGDATA_UPDATE:String = "emgDataUpdate";
		
		
		
		public var myo:Myo;
		public var errorMessage:String;
		
		public function MyoEvent(type:String, myo:Myo = null, bubbles:Boolean=false, cancelable:Boolean=false) 
		{ 
			super(type, bubbles, cancelable);
			this.myo = myo;
		} 
		
		public override function clone():Event 
		{ 
			return new MyoEvent(type, myo, bubbles, cancelable);
		} 
		
		public override function toString():String 
		{ 
			return formatToString("MyoEvent", "type", "myoID", "bubbles", "cancelable", "eventPhase"); 
		}
		
	}
	
}