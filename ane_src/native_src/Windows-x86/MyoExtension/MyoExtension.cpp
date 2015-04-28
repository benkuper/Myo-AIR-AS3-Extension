// MyoExtension.cpp : Defines the exported functions for the DLL application.
//

#include "MyoExtension.h"// MyoExtension.cpp : Defines the exported functions for the DLL application.
//

#include "myo/myo.hpp"
#include <iostream>
#include "pthread.h"

using namespace std;
using namespace myo;

pthread_t runThread;
bool exitRunThread;



class MyoData
{
	public :
	Myo * myo;

	string id;

	string pose;
	double yaw;
	double pitch;
	double roll;

	double accel[3];
	double gyro[3];

	int8_t emgData[8];
	bool emgEnabled;

	bool connected;
	
};

vector<MyoData *> myoDatas;



string getMyoID(Myo *myo)
{

	return std::to_string((int)myo);
}

MyoData * addMyo(Myo * myo)
{
	printf("* Add Myo\n");
	MyoData * data = new MyoData();
	data->myo = myo;
	data->id = getMyoID(myo); //->macAddressAsString();
	data->yaw = 0;
	data->pitch = 0;
	data->roll = 0;
	data->pose = "rest";
	myoDatas.push_back(data);

	data->emgEnabled = false;
	for(int i=0;i<8;i++) data->emgData[i] = 0;

	//myo->setStreamEmg(Myo::streamEmgEnabled);

	return data;
}



MyoData * getMyoData(Myo * myo, bool createIfNotExist = true)
{	
	if(exitRunThread) return NULL;

	for (size_t i = 0; i < myoDatas.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
		if (myoDatas[i]->myo == myo) {
            return myoDatas[i];
        }
    }

	if(createIfNotExist) return addMyo(myo);
	printf("Myo Not found by handle : %s\n",getMyoID(myo).c_str());
	return NULL;
}

int getMyoIndex(Myo *myo)
{
	if(exitRunThread) return -1;

	for (size_t i = 0; i < myoDatas.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
		if (myoDatas[i]->myo == myo) {
            return i;
        }
    }

	printf("Myo Not found : %s\n",getMyoID(myo).c_str());
	return -1;
}


MyoData * getMyoByMacAddress(const char * macAddress)
{
	if(exitRunThread) return NULL;
	//printf("Get Myo by aAddress : %s\n",macAddress);
	for (size_t i = 0; i < myoDatas.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
		if (strcmp(myoDatas[i]->id.c_str(),macAddress) == 0) {
            return myoDatas[i];
        }
    }

	//printf("Myo Not found by mac address : %s\n",macAddress);
	return NULL;
}



void removeMyo(Myo * myo)
{
	printf("Remove Myo\n");
	int id = getMyoIndex(myo);
	if(id != -1) myoDatas.erase(myoDatas.begin()+id);
}



class MyoListener : public myo::DeviceListener {
public:
    MyoListener()
    {
		printf("Myo listener creation\n");
    }

	void onPair(myo::Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)
    {
		printf("Myo onPair \n");
		addMyo(myo);

		// Now that we've added it to our list, get our short ID for it and print it out.
		//printf("=> assigned ID : %s\n",data->id.c_str());

    }

	

	void onEmgData(Myo *myo, uint64_t timestamp, const int8_t * emg)
	{
		//printf("Got emg data \n");
		MyoData * mData = getMyoData(myo);
		if(mData != NULL)
		{
			for(int i=0;i<8;i++) mData->emgData[i] = emg[i];
		}
	}

	void onUnpair (Myo *myo, uint64_t timestamp)
	{
		printf("Myo unpair\n");
		removeMyo(myo);
	}

	 void onConnect(Myo* myo, uint64_t timestamp,FirmwareVersion firmwareVersion)
    {
		printf("Myo %s has connected\n",getMyoID(myo).c_str());
		MyoData * mData = getMyoData(myo);
		if(mData != NULL) mData->connected = true;
    }


	void onDisconnect(Myo* myo, uint64_t timestamp)
    {
		printf("Myo %s has disconnected\n",getMyoID(myo).c_str());
		MyoData * mData = getMyoData(myo);
		if(mData != NULL) mData->connected = false;

    }

    void onPose(Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
		
		printf("Myo %s has detected pose : %s\n",getMyoID(myo).c_str(),pose.toString().c_str());
		MyoData * mData = getMyoData(myo);
		if(mData != NULL) mData->pose = pose.toString();
		
    }

   

    

    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& quat)
    {
		//printf("Orientation data\n");

		MyoData * mData = getMyoData(myo);
		if(mData == NULL) return;


        using std::atan2;
        using std::asin;
        using std::sqrt;

        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        double roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));

        double pitch = asin(2.0f * (quat.w() * quat.y() - quat.z() * quat.x()));

        double yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

		
		mData->yaw = yaw;
		mData->pitch = pitch;
		mData->roll = roll;

		//printf("> yaw %f\n",yaw);//,mData->data.yaw,mData.pitch,mData.roll);
    }

	void onAccelerometerData (Myo *myo, uint64_t timestamp, const Vector3< float > &accel)
	{
		//printf("Accel data ! %f %f %f",accel[0],accel[1],accel[2]);
		MyoData * mData = getMyoData(myo);

		mData->accel[0] = accel[0];
		mData->accel[1] = accel[1];
		mData->accel[2] = accel[2];
	}

	void onGyroscopeData (Myo *myo, uint64_t timestamp, const Vector3< float > &gyro)
	{

		//printf("Gyro data ! %f %f %f",gyro[0],gyro[1],gyro[2]);
		MyoData * mData = getMyoData(myo);

		mData->gyro[0] = gyro[0];
		mData->gyro[1] = gyro[1];
		mData->gyro[2] = gyro[2];
	}
};


MyoListener * mListener;
Hub * hub = NULL;
bool hubIsReady = false;
bool hubWaitingForInit = true;
//Run thread

void *MyoRunThread(FREContext ctx)
{
	printf("myo run thread creation and init !\n");
	
	hubIsReady = false;
	hubWaitingForInit = true;

	try
	{
	
		printf("Add listener\n");
		hub = new Hub();
		mListener = new MyoListener();
		hub->addListener(mListener);
		hubWaitingForInit = false;
	}catch(exception e)
	{
		printf("Error creating Hub or Listener : %s\n",e.what());
		FREDispatchStatusEventAsync(ctx,(const uint8_t*)"error",(const uint8_t *)"connect");
		hubWaitingForInit = false;
		return 0;
	}

	hubWaitingForInit = false;
	hubIsReady = true;

	printf("Hub is ready, Launch Hub run loop\n");
	while(!exitRunThread)
	{
		try
		{
			hub->run(10);
		}catch(exception e)
		{
			printf("Error running hub : %s\n",e.what());
		}
	}

	//hub->removeListener(&mListener);
	int val = 0;
	//pthread_exit(&val);
	
   printf("Exit Run Thread !\n");
   hub = NULL;
	mListener = NULL;
   return 0;
}

extern "C"
{

	FREObject init(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		
		printf("MyoExtension :: init\n");

		bool success = true;

		try
		{
			int pThreadResult = pthread_create(&runThread, NULL, MyoRunThread,ctx);

			if (pThreadResult){
				printf("MyoExtension :: Error on Myo run thread creation\n");
			}
			
		}catch(const std::exception& e)
		{
			printf("MyoExtension :: Error on Myo Init (%s)\n",e.what());

			FREDispatchStatusEventAsync(ctx,(const uint8_t *)"error",(const uint8_t *)e.what());
			success = false;
		}
		
		FREObject result;
		FRENewObjectFromBool(success,&result);
		return result;

	}

	FREObject updateAllMyoData(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		if(exitRunThread || !hubIsReady) return NULL; //thread is exiting or already exited

		//return NULL; //temp

		FREObject controller = argv[0];


		int dLen = myoDatas.size();
		for(int i=0;i < dLen ;i++)
		{
		
			//printf("update Myo %i --> ",i);
			MyoData * mData = myoDatas[i];

			const uint8_t * mID = reinterpret_cast<const uint8_t*>(&(mData->id.c_str())[0]);
			int midl = mData->id.length(); //to change


			const uint8_t * pID = reinterpret_cast<const uint8_t*>(&(mData->pose.c_str())[0]);
			int pidl = mData->pose.length(); //to change

			//printf("MID= %s\n",mID);

			const int numParameters = 12;
			FREObject data[numParameters];
			
			FRENewObjectFromUTF8(midl,mID,&data[0]); //id
			FRENewObjectFromBool(mData->connected,&data[1]); //id

			FRENewObjectFromUTF8(pidl,pID,&data[2]); //pose
			FRENewObjectFromDouble(mData->yaw,&data[3]); //yaw
			FRENewObjectFromDouble(mData->pitch,&data[4]); //pitch
			FRENewObjectFromDouble(mData->roll,&data[5]); //roll

			FRENewObjectFromDouble(mData->accel[0],&data[6]); //accelX
			FRENewObjectFromDouble(mData->accel[1],&data[7]); //accelY
			FRENewObjectFromDouble(mData->accel[2],&data[8]); //accelZ

			FRENewObjectFromDouble(mData->gyro[0],&data[9]); //gyroX
			FRENewObjectFromDouble(mData->gyro[1],&data[10]); //gyroY
			FRENewObjectFromDouble(mData->gyro[2],&data[11]); //gyroZ

			 
			FREObject res;
			FREResult fre = FRECallObjectMethod(controller,(const uint8_t*)"updateMyoData",numParameters,data,&res,NULL); //AS3 updateMyoData(id:String,pose:String,yaw:Number,pitch:Number,roll:Number)
			 
			if(mData->emgEnabled)
			{
				FREObject emgData[9];
				FRENewObjectFromUTF8(midl,mID,&emgData[0]); //id
				for(int e=0;e<8;e++) 
				{
					//printf("%i, ",mData->emgData[e]);// 
					FRENewObjectFromInt32(mData->emgData[e],&emgData[e+1]);
				}
				//printf("\n");
				fre = FRECallObjectMethod(controller,(const uint8_t*)"updateMyoEmgData",9,emgData,&res,NULL); //AS3 updateMyoEmgData(data1:int,data2:int,...,data8:int)
			}

			//printf("Call method : %i\n",fre);
		}
		
		
		FREObject result;
		FRENewObjectFromBool(true,&result);
		return result;
		
	}

	FREObject setStreamEMG(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool mResult = false;
		const uint8_t* macAddress;
		uint32_t macLength;
		FREGetObjectAsUTF8(argv[0],&macLength,&macAddress);

		uint32_t enabled;
		FREGetObjectAsBool(argv[1], &enabled);
		Myo * myo = getMyoByMacAddress((const char *)macAddress)->myo;

		if(myo != NULL)
		{
			myo->setStreamEmg(enabled > 0?Myo::streamEmgEnabled:Myo::streamEmgDisabled);
			
			MyoData * mData = getMyoData(myo);
			mData->emgEnabled = enabled > 0;

			mResult = true;
		}
			
		FREObject result;
		FRENewObjectFromBool(mResult,&result);
		return result;
	}


	FREObject setLockingPolicy(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		
		uint32_t lockPolicy;
		FREGetObjectAsBool(argv[0], &lockPolicy);
		printf("Set locking Policy %s\n",lockPolicy?"standard":"none");

		bool success = true;
		while(hubWaitingForInit)
		{
			//do nothing
			printf("waiting for init... %i\n",hubIsReady);
		}

		if(hubIsReady)
		{
			printf("hub is ready, lock here\n");
			hub->setLockingPolicy(lockPolicy?Hub::lockingPolicyStandard:Hub::lockingPolicyNone);
		}else
		{
			printf("Hub is not ready\n");
			success = false;
		}

		FREObject result;
		FRENewObjectFromBool(success,&result);
		return result;
	}



	FREObject setLock(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		bool mResult = false;
		const uint8_t* macAddress;
		uint32_t macLength;
		FREGetObjectAsUTF8(argv[0],&macLength,&macAddress);

		uint32_t enabled;
		FREGetObjectAsBool(argv[1], &enabled);

		uint32_t type;
		FREGetObjectAsBool(argv[2], &type);
		

		Myo * myo = getMyoByMacAddress((const char *)macAddress)->myo;

		if(myo != NULL)
		{
			if(enabled) myo->lock();
			else myo->unlock(type?Myo::unlockTimed:Myo::unlockHold);
			mResult = true;
		}
			
		FREObject result;
		FRENewObjectFromBool(mResult,&result);
		return result;
	}


	
	FREObject vibrate(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
			bool mResult = false;

			const uint8_t* macAddress;
			uint32_t macLength;
			FREGetObjectAsUTF8(argv[0],&macLength,&macAddress);
			Myo * myo = getMyoByMacAddress((const char *)macAddress)->myo;

			if(myo != NULL)
			{
				int vibType;
				FREGetObjectAsInt32(argv[1],&vibType);
				Myo::VibrationType vibrationType = myo->vibrationShort; //safe assigning

				if(vibType == 0) vibrationType = myo->vibrationShort;
				else if(vibType == 1) vibrationType = myo->vibrationMedium;
				else if(vibType == 2) vibrationType = myo->vibrationLong;

				myo->vibrate(vibrationType);
				mResult = true;
			}
			
			FREObject result;
			FRENewObjectFromBool(mResult,&result);
			return result;
	}

	FREObject clean(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("Clean !\n");
		exitRunThread = true;
		mListener = NULL;
		hub = NULL;
		/*
		try
		{
			pthread_cancel(runThread);
		}catch(exception e)
		{
			printf("Thread already exited !");
		}
		*/

		FREObject result;
		FRENewObjectFromBool(true,&result);
		return result;
	}

	// Flash Native Extensions stuff
	void MyoContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToSet,  const FRENamedFunction** functionsToSet) { 

		printf("MyoExtension v0.8.1 by Ben kuper, based on MyoSDK v0.8.1\n");
		
		*numFunctionsToSet = 7; 

 
		FRENamedFunction* func = (FRENamedFunction*)malloc(sizeof(FRENamedFunction)*2); 

		func[0].name = (const uint8_t*)"init";  
		func[0].functionData = NULL; 
		func[0].function = &init; 
		
		func[1].name = (const uint8_t*)"update"; 
		func[1].functionData = NULL; 
		func[1].function = &updateAllMyoData; 

		func[2].name = (const uint8_t*)"vibrate"; 
		func[2].functionData = NULL; 
		func[2].function = &vibrate; 


		func[3].name = (const uint8_t*)"stop"; 
		func[3].functionData = NULL; 
		func[3].function = &clean; 

		func[4].name = (const uint8_t*)"setStreamEMG"; 
		func[4].functionData = NULL; 
		func[4].function = &setStreamEMG; 
		
		func[5].name = (const uint8_t*)"setLockingPolicy"; 
		func[5].functionData = NULL; 
		func[5].function = &setLockingPolicy; 
		
		func[6].name = (const uint8_t*)"setLock"; 
		func[6].functionData = NULL; 
		func[6].function = &setLock; 
		
		
		*functionsToSet = func; 
	}


	void MyoContextFinalizer(FREContext ctx) 
	{
		exitRunThread = true;
		return;
	}

	void MyoExtInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer) 
	{
		*ctxInitializer = &MyoContextInitializer;
		*ctxFinalizer   = &MyoContextFinalizer;
	}

	void MyoExtFinalizer(void* extData) 
	{
		return;
	}
}


