package 
{
	import benkuper.nativeExtensions.Myo;
	import benkuper.nativeExtensions.MyoController;
	import benkuper.nativeExtensions.MyoEvent;
	import flash.display.Graphics;
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.KeyboardEvent;
	import flash.ui.Keyboard;
	
	/**
	 * ...
	 * @author Ben Kuper
	 */
	public class MyoControllerDemo extends Sprite 
	{
		private var myoController:MyoController;
		
		private var myo:Myo;
		
		private var g:Shape;
		private var color:uint;
		
		public function MyoControllerDemo():void 
		{
			g = new Shape();
			addChild(g);
			g.x = 300;
			g.y =  100;
			myoController = new MyoController();
			myoController.addEventListener(MyoEvent.MYO_PAIRED, myoPaired);
			myoController.setLockingPolicy(false);
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, keyDown);
		}
		
		private function keyDown(e:KeyboardEvent):void 
		{
			switch(e.keyCode)
			{
				case Keyboard.L:
					myo.setLock(true);
					break;
					
				case Keyboard.U:
					myo.setLock(false,e.shiftKey);
					break;
					
				case Keyboard.P:
					trace("set locking policy");
					myoController.setLockingPolicy(e.shiftKey);
					break;
					
				case Keyboard.V:
					myo.vibrate(e.shiftKey?Myo.VIBRATION_LONG:Myo.VIBRATION_SHORT);
					break;
				
				case Keyboard.E:
					myo.setStreamEMG(e.shiftKey);
					break;
				
			}
		}
		
		private function myoPaired(e:MyoEvent):void 
		{
			trace("Myo is paired");
			myo = e.myo;
			myo.setLock(false);
			myo.addEventListener(MyoEvent.MYO_CONNECTED, myoConnected);
			myo.addEventListener(MyoEvent.MYO_DISCONNECTED, myoDisconnected);
			myo.addEventListener(MyoEvent.ORIENTATION_UPDATE, myoOrientationUpdate);
			myo.addEventListener(MyoEvent.POSE_UPDATE, myoPoseUpdate);
			myo.addEventListener(MyoEvent.EMGDATA_UPDATE, myoEmgDataUpdate);
		}
		
		private function myoEmgDataUpdate(e:MyoEvent):void 
		{
			trace("Main :: emg data update !", e.myo.emgData);
		}
		
		private function myoConnected(e:MyoEvent):void 
		{
			trace("Main :: myo connected !", e.myo.id);
		}
		
		private function myoDisconnected(e:MyoEvent):void 
		{
			trace("Main :: myo disconnected !", e.myo.id);
		}
		
		private function myoOrientationUpdate(e:MyoEvent):void 
		{
			//trace("Main :: myo orientation update !", e.myo.yaw, e.myo.pitch, e.myo.roll);
			
			g.graphics.clear();
			g.graphics.beginFill(color);
			g.graphics.drawRect(0, 20, e.myo.yaw*50,20);

			g.graphics.drawRect(0, 50, e.myo.pitch * 50, 20);
			
			g.graphics.drawRect(0, 80, e.myo.roll*50,20);
			
			
			g.graphics.drawRect(0, 110, e.myo.accel[0] * 50, 20);
			g.graphics.drawRect(0, 140, e.myo.accel[1] * 50, 20);
			g.graphics.drawRect(0, 170, e.myo.accel[2] *50,20);
			
			
			g.graphics.drawRect(0, 200, e.myo.gyro[0] * 50, 20);
			g.graphics.drawRect(0, 230, e.myo.gyro[1] * 50, 20);
			g.graphics.drawRect(0, 260, e.myo.gyro[2] *50,20);
			
			
			g.graphics.endFill();
			
		}
		
		private function myoPoseUpdate(e:MyoEvent):void 
		{
			trace("Main :: myo pose update !", e.myo.pose);
			
			switch(e.myo.pose)
			{
				case Myo.POSE_NONE:
					color = 0x000000;
					break;
					
					
				case Myo.POSE_FIST:
					color = 0xff0000;
					myo.vibrate();
					break;
					
				case Myo.POSE_FINGERS_SPREAD:
					color = 0xffff00;
					break;
					
				case Myo.POSE_WAVE_IN:
					color = 0x00ffff;
					break;
					
				case Myo.POSE_WAVE_OUT:
					color = 0x00ff00;
					break;
					
				case Myo.POSE_DOUBLE_TAP:
					color = 0xff00ff;
					break;
			}
			
			graphics.clear();
			graphics.beginFill(color);
			graphics.drawCircle(50,50,20);
			graphics.endFill();
		}
		
	}
	
}