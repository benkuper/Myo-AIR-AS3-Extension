package benkuper.nativeExtensions
{
	import flash.desktop.NativeApplication;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.StatusEvent;
	
	import flash.events.TimerEvent;
	import flash.external.ExtensionContext;
	import flash.system.Capabilities;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	
	/**
	 * ...
	 * @author Ben Kuper
	 */
	public class MyoController extends EventDispatcher 
	{
		
		private var extContext:ExtensionContext; 
		public static var instance:MyoController;
		
		public var nativeTest:int;
		public var myos:Vector.<Myo>;	
		
		private var updateTimer:Timer;
		
		public function MyoController() 
		{		
			
			trace("** MyoController - Ben Kuper **");
			instance = this;
			
			extContext = ExtensionContext.createExtensionContext("benkuper.nativeExtensions.MyoController", "myo");
			
			if (extContext == null)
			{
				trace("3:[MyoController] Problem with creating ANE");
				return;
			}
			
			myos = new Vector.<Myo>();
			
			extContext.addEventListener(StatusEvent.STATUS, onStatus);
			
			extContext.call("init");
			
			trace("[MyoController] ** Extension Context created and init **");
			
			
			//cleaning handling
			NativeApplication.nativeApplication.addEventListener(Event.EXITING, appExiting);
			
			
			updateTimer = new Timer(10); //100 fps
			updateTimer.addEventListener(TimerEvent.TIMER, updateTimerHandler);
			updateTimer.start();
		}
		
		private function updateTimerHandler(e:TimerEvent):void 
		{
			//trace("call update");
			extContext.call("update",this);
		}
		
		
		//events
		
		private function onStatus(e:StatusEvent):void 
		{
			//trace("[MyoController] Status Event from Myo Extension :", e.level, e.code);
			
			switch(e.code)
			{
				case "data":
					extContext.call("update",this);
					break;
			}
			
		}
		
		public function updateMyoData(id:String, pose:String, yaw:Number, pitch:Number, roll:Number,
												accelX:Number,accelY:Number,accelZ:Number,gyroX:Number,gyroY:Number,gyroZ:Number):void
		{
			//trace("[MyoController] Update myo data !", id, pose, yaw, pitch, roll);
			var myo:Myo = getMyoByID(id);
			if (myo == null) myo = addMyo(id);
			
			myo.updateData(pose, yaw, pitch, roll,accelX,accelY,accelZ,gyroX,gyroY,gyroZ);
		}
		
		private function addMyo(myoID:String):Myo
		{
			var m:Myo = getMyoByID(myoID);
			if (m != null) return m;
			
			m = new Myo(myoID);
			myos.push(m);
			
			m.isTrained = isMyoTrained(m);
			//trace("[MyoController] Add Myo : ", m.id, m.isTrained);
			dispatchEvent(new MyoEvent(MyoEvent.MYO_PAIRED, m));
			
			return m;
		}
		
		private function removeMyo(myo:Myo):void
		{
			myos.splice(myos.indexOf(myo), 1);
		}
		
		public function vibrate(id:String, vibrationType:int):void 
		{
			trace("[MyoController] vibrate");
			extContext.call("vibrate", id, vibrationType);
		}
		
		public function isMyoTrained(myo:Myo):Boolean
		{
			return extContext.call("isMyoTrained", myo.id) as Boolean;
		}
		
		private function getMyoByID(id:String):Myo
		{
			for each(var m:Myo in myos) if (m.id == id) return m;
			
			return null;
		}
		
		//Cleaning
		
		private function appExiting(e:Event):void 
		{
			clean();
		}
		
		public function clean():void
		{
			instance = null;
			
			if (extContext == null)
			{ 
				trace("[MyoController::clean] ExtContext is null, not cleaning");
				return;
			}
			
			updateTimer.reset();
			updateTimer.removeEventListener(TimerEvent.TIMER, updateTimerHandler);
			updateTimer = null;
			
			extContext.removeEventListener(StatusEvent.STATUS, onStatus);
			extContext.call("stop");
			extContext.dispose();
			extContext = null;
		}
		
		
		
	}

}