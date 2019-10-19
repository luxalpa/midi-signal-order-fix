#pragma comment(lib, "winmm.lib")

#include "StdAfx.h"
#include <iostream>
#include "MIDIInDevice.h"
#include "MIDIOutDevice.h"
#include "ShortMsg.h"

using midi::CMIDIInDevice;
using midi::CMIDIReceiver;
using midi::CMIDIOutDevice;

midi::CMIDIOutDevice OutDevice;
bool pressedKeys[16][128];

// My class for receiving MIDI messages
class MyReceiver : public CMIDIReceiver
{
public:
    // Receives short messages
    void ReceiveMsg(DWORD Msg, DWORD TimeStamp);

    // Receives long messages
    void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
    {}

    // Called when an invalid short message is received
    void OnError(DWORD Msg, DWORD TimeStamp)
    { std::cout << "Invalid short message received!\n"; }

    // Called when an invalid long message is received
    void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) {}
};

// Function called to receive short messages
void MyReceiver::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
    midi::CShortMsg ShortMsg(Msg, TimeStamp);
	int key = ShortMsg.GetData1();
	int channel = ShortMsg.GetChannel();
	if(ShortMsg.GetCommand() == 144) {
		if(pressedKeys[channel][key]) {
			ShortMsg.SetMsg(128, channel, key, ShortMsg.GetData2());
			ShortMsg.SendMsg(OutDevice);
			ShortMsg.SetMsg(144, channel, key, ShortMsg.GetData2());
		}
		pressedKeys[channel][key] = true;
	} else if(ShortMsg.GetCommand() == 128) {
		pressedKeys[channel][key] = false;
	}
	OutDevice.SendMsg(Msg);
	
	//if(ShortMsg.GetCommand() == 144) {
	//	Beep(523,500);
	//    //keybd_event(vKey, MapVirtualKey(vKey, 0), 0, 0);
	//}
	//else if(ShortMsg.GetCommand() == 128) {
	//	//keybd_event(vKey, MapVirtualKey(vKey, 0), KEYEVENTF_KEYUP, 0);
	//}
	//if(ShortMsg.GetCommand() != 240) {
		////std::cout << (unsigned char) ShortMsg.GetData1();
	 //   std::cout << "Command: " <<
		//static_cast<int>(ShortMsg.GetCommand());
	 //   std::cout << "\nChannel: " <<
		//static_cast<int>(ShortMsg.GetChannel());
	 //   std::cout << "\nDataByte1: " <<
		//static_cast<int>(ShortMsg.GetData1());
	 //   std::cout << "\nDataByte2: " <<
		//static_cast<int>(ShortMsg.GetData2());
		//std::cout << "\nTimeStamp: " <<
		//static_cast<int>(ShortMsg.GetTimeStamp());
	 //   std::cout << "\n\n";
	//}
	//if(ShortMsg.GetCommand() == 144) {
	//	std::cout << static_cast<int>(ShortMsg.GetTimeStamp()) << " X " << static_cast<int>(ShortMsg.GetData1()) << " " << static_cast<int>(ShortMsg.GetData2()) << "\n";
	//	
	//} else
	//if(ShortMsg.GetCommand() == 128) {
	//	std::cout << static_cast<int>(ShortMsg.GetTimeStamp()) << " - " << static_cast<int>(ShortMsg.GetData1()) << " " << static_cast<int>(ShortMsg.GetData2()) << "\n";
	//} else 
	//	std::cout << static_cast<int>(ShortMsg.GetTimeStamp()) << " " << static_cast<int>(ShortMsg.GetCommand()) << " > " << static_cast<int>(ShortMsg.GetData1()) << " " << static_cast<int>(ShortMsg.GetData2()) << "\n";

}

int main(void)
{
	for(int i = 0; i < 16; i++) {
		for(int ii = 0; ii < 128; ii++) {
			pressedKeys[i][ii] = false;
		}
	}
    try
    {
        // Make sure there is a MIDI input device present
        if(CMIDIInDevice::GetNumDevs() > 0)
        {
            MyReceiver Receiver;
            CMIDIInDevice InDevice(Receiver);

            // We'll use the first device - we're not picky here
            InDevice.Open(8);
			OutDevice.Open(2);

            // Start recording
            InDevice.StartRecording();

            // Wait for some MIDI messages
            ::Sleep(1000*3600*10);

            // Stop recording
            InDevice.StopRecording();

            // Close the device
            InDevice.Close();
        }
        else
        {
            std::cout << "No MIDI input devices present!\n";
        }
    }
    catch(const std::exception &Err)
    {
        std::cout << Err.what();
    }

    return 0;
}