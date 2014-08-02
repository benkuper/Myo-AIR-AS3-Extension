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
	
};

vector<MyoData *> myoDatas;

string getMyoID(Myo *myo)
{
	return std::to_string((int)myo);
}

MyoData * getMyoData(Myo * myo)
{
	for (size_t i = 0; i < myoDatas.size(); ++i) {
        // If two Myo pointers compare equal, they refer to the same Myo device.
		if (myoDatas[i]->myo == myo) {
            return myoDatas[i];
        }
    }

	printf("Myo Not found by mac address : %s\n",getMyoID(myo).c_str());
	return NULL;
}




MyoData * getMyoByMacAddress(const char * macAddress)
{
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


class MyoListener : public myo::DeviceListener {
public:
    MyoListener()
    {
		printf("Myo listener creation\n");
    }

	void onPair(myo::Myo* myo, uint64_t timestamp, FirmwareVersion firmwareVersion)
    {
		MyoData * data = new MyoData();
		data->myo = myo;
		data->id = getMyoID(myo); //->macAddressAsString();
		data->yaw = 0;
		data->pitch = 0;
		data->roll = 0;
		data->pose = "rest";
		myoDatas.push_back(data);


        // Now that we've added it to our list, get our short ID for it and print it out.
		printf("Myo paired, assigned ID : %s\n",data->id.c_str());

        // Check whether this Myo device has been trained. It will only provide pose information if it's been trained.
        /*
		if (myo->isTrained()) {
			printf("Myo %s is trained\n",myo->macAddressAsString().c_str());
        } else {
			printf("Myo %s is NOT trained\n",myo->macAddressAsString().c_str());
        }
		*/
    }

	 void onConnect(Myo* myo, uint64_t timestamp,FirmwareVersion firmwareVersion)
    {
		printf("Myo %s has connected\n",getMyoID(myo).c_str());
    }

    void onPose(Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
		//int myoID = identifyMyo(myo);
		
		printf("Myo %s has detected pose : %s\n",getMyoID(myo).c_str(),pose.toString().c_str());
		
		MyoData * mData = getMyoData(myo);
		mData->pose = pose.toString();
		
    }

   

    void onDisconnect(Myo* myo, uint64_t timestamp)
    {
		printf("Myo %s has disconnected\n",getMyoID(myo));
    }

    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(Myo* myo, uint64_t timestamp, const Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;

        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        double roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));

        double pitch = asin(2.0f * (quat.w() * quat.y() - quat.z() * quat.x()));

        double yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

		MyoData * mData = getMyoData(myo);

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




//Run thread

void *MyoRunThread(FREContext ctx)
{
	printf("myo run thread creation and init !\n");
	myo::Hub hub;
	//hub.addMyo(
	printf("Hub :: pairWithAnyMyo\n");

	MyoListener mListener;
	hub.addListener(&mListener);

   while(!exitRunThread)
   {
	   hub.run(10);

	   //FREDispatchStatusEventAsync(ctx,(const uint8_t *)"data",(const uint8_t *)"myo");
   }

   printf("Exit Run Thread !\n");
   return 0;
}

extern "C"
{

	FREObject init(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		
		printf("Myo Extension :: init\n");
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
		if(exitRunThread) return NULL; //thread is exiting or already exited

		//printf("Update Native\n");
		
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

			FREObject data[11];
			FRENewObjectFromUTF8(midl,mID,&data[0]); //id
			FRENewObjectFromUTF8(pidl,pID,&data[1]); //pose
			FRENewObjectFromDouble(mData->yaw,&data[2]); //yaw
			FRENewObjectFromDouble(mData->pitch,&data[3]); //pitch
			FRENewObjectFromDouble(mData->roll,&data[4]); //roll

			FRENewObjectFromDouble(mData->accel[0],&data[5]); //accelX
			FRENewObjectFromDouble(mData->accel[1],&data[6]); //accelY
			FRENewObjectFromDouble(mData->accel[2],&data[7]); //accelZ

			FRENewObjectFromDouble(mData->gyro[0],&data[8]); //gyroX
			FRENewObjectFromDouble(mData->gyro[1],&data[9]); //gyroY
			FRENewObjectFromDouble(mData->gyro[2],&data[10]); //gyroZ


			FREObject res;
			FREResult fre = FRECallObjectMethod(controller,(const uint8_t*)"updateMyoData",11,data,&res,NULL); //AS3 updateMyoData(id:String,pose:String,yaw:Number,pitch:Number,roll:Number)
			
			//printf("Call method : %i\n",fre);
		}
		
		
		FREObject result;
		FRENewObjectFromBool(true,&result);
		return result;
		
	}

	FREObject vibrate(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
			const uint8_t* macAddress;
			uint32_t macLength;
			FREGetObjectAsUTF8(argv[0],&macLength,&macAddress);
			Myo * myo = getMyoByMacAddress((const char *)macAddress)->myo;
			
			int vibType;
			FREGetObjectAsInt32(argv[1],&vibType);
			Myo::VibrationType vibrationType = myo->vibrationShort; //safe assigning

			if(vibType == 0) vibrationType = myo->vibrationShort;
			else if(vibType == 1) vibrationType = myo->vibrationMedium;
			else if(vibType == 2) vibrationType = myo->vibrationLong;

			myo->vibrate(vibrationType);

			FREObject result;
			FRENewObjectFromBool(true,&result);
			return result;
	}

	FREObject isMyoTrained(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		/*
			const uint8_t* macAddress;
			uint32_t macLength;
			FREGetObjectAsUTF8(argv[0],&macLength,&macAddress);

			Myo * myo = getMyoByMacAddress((const char *)macAddress)->myo;
			*/

			FREObject result;
			FRENewObjectFromBool(true,&result);
			return result;
	}


	FREObject clean(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		printf("Clean !\n");
		exitRunThread = true;
		
		/*try
		{
			pthread_cancel(&runThread);
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

		printf("** MyoExtension v0.3 by Ben Kuper **\n\n");

		*numFunctionsToSet = 5; 

 
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

		func[3].name = (const uint8_t*)"isMyoTrained"; 
		func[3].functionData = NULL; 
		func[3].function = &isMyoTrained; 

		func[4].name = (const uint8_t*)"stop"; 
		func[4].functionData = NULL; 
		func[4].function = &clean; 
		
		*functionsToSet = func; 
	}


	void MyoContextFinalizer(FREContext ctx) 
	{
		exitRunThread = true;
		try
		{
			pthread_cancel(runThread);
		}catch(exception e)
		{
			printf("Thread already exited !");
		}
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


