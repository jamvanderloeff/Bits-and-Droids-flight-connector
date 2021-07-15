// BitsAndDroidsInputHandler.cpp : This file contains the 'main' function.
// Program execution begins and ends there.
//
#include "InputWorker.h"

#include <Windows.h>
#include <headers/SerialReader.h>
#include <headers/constants.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <tchar.h>

#include <cstdio>
#include <future>
#include <iostream>
#include <string>

#include "InputMapper.h"
#include "InputSwitchHandler.h"
#include "headers/SerialPort.hpp"
#include "headers/SimConnect.h"

#define Bcd2Dec(BcdNum) HornerScheme(BcdNum, 0x10, 10)
#define Dec2Bcd(DecNum) HornerScheme(DecNum, 10, 0x10)

#define DATA_LENGTH 255

// Declare a global object
SerialPort *arduinoInput[10];

HANDLE hInputSimConnect = NULL;

// Definition of the client data area format
double data = 1.;
using namespace std;
InputEnum defs = InputEnum();
InputWorker::InputWorker() {}
InputMapper mapper = InputMapper();
InputSwitchHandler handler = InputSwitchHandler();

enum GROUP_ID {
  GROUP0,
  GROUP_A = 1,
};

int x = 0;
enum DATA_REQUEST_ID {

  REQUEST_THROTTLE,
  REQUEST_MIXTURE,
  REQUEST_PROPS,
  REQUEST_1 = 10,
};

void InputWorker::MyDispatchProcInput(SIMCONNECT_RECV *pData, DWORD cbData,
                                      void *pContext) {
  switch (pData->dwID) {
    case SIMCONNECT_RECV_ID_QUIT: {
      // quit = 1;
      break;
    }
    case SIMCONNECT_RECV_ID_CLIENT_DATA: {
      // Cast incoming data into interpretable format for this event.
      SIMCONNECT_RECV_CLIENT_DATA *pObjData =
          (SIMCONNECT_RECV_CLIENT_DATA *)pData;

      printf("Request ID = %d.\n", pObjData->dwRequestID);
      break;
    }

    default:
      break;
  }
}

enum INPUT_ID {
  INPUT0,
};

void InputWorker::inputEvents() {
  HRESULT hr;
  keys = *settingsHandler.retrieveKeys("inputCom");
  for (int i = 0; i < keys.size(); i++) {
    const char *savedPort;
    savedPort = settingsHandler.retrieveSetting("inputCom", keys[i])
                    ->toString()
                    .toStdString()
                    .c_str();

    arduinoInput[i] = new SerialPort(savedPort);
    std::cout << i << "Is connected: " << arduinoInput[i]->isConnected()
              << std::endl;
  }

  while (!connected && !abortInput) {
    cout << "YUPS" << endl;
    if (SUCCEEDED(SimConnect_Open(&hInputSimConnect, "data", NULL, 0, 0, 0))) {
      handler.connect = hInputSimConnect;

      handler.object = SIMCONNECT_OBJECT_ID_USER;
      mapper.mapEvents(hInputSimConnect);

      connected = true;
      Sleep(5);
      while (!abortInput) {
        for (int i = 0; i < keys.size(); i++) {
          cout << arduinoInput[i]->isConnected() << "wh" << endl;
          const auto hasRead = arduinoInput[i]->readSerialPort(
              handler.receivedString[i], DATA_LENGTH);

          if (hasRead) {
            if (connected) {
              emit updateLastStatusUI("Connected to game");
              handler.switchHandling(i);
            }
            lastVal = handler.receivedString[i];
          }

          emit updateLastValUI(QString::fromStdString(lastVal));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }

      emit updateLastStatusUI("Connection closed");
      if (connected) {
        SimConnect_Close(hInputSimConnect);
      } else {
        emit updateLastStatusUI("No connection to the game");
        sleep(100);
      }
    }
  }
  for (int i = 0; i < keys.size(); i++) {
    if (arduinoInput[i]->isConnected()) {
      arduinoInput[i]->closeSerial();
    }
  }
  cout << "we are here" << endl;
  Sleep(1);
  quit();
}

InputWorker::~InputWorker() {
  for (int i = 0; i < keys.size(); i++) {
    if (arduinoInput[i]->isConnected()) {
      arduinoInput[i]->closeSerial();
    }
  }
  abortInput = true;
  connected = false;
  mutex.lock();
  condition.wakeOne();
  mutex.unlock();
}
