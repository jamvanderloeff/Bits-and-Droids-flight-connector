#ifndef INPUTWORKER_H
#define INPUTWORKER_H
#include <qmutex.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qthread.h>
#include <qwaitcondition.h>
#include <tchar.h>
#include <windows.h>

#include <QObject>
#include <string>

#include "SimConnect.h"
#include "stdio.h"
/*!
  \class InputWorker
  \brief The InputWorker class
 */
class InputWorker : public QThread {
 public:
  InputWorker();
  void run() override { inputEvents(); }
  ~InputWorker();
  QMutex mutex;
  QWaitCondition condition;
  bool abortInput = false;
  bool advanced = false;
  bool props = true;

 private slots:
  void switchHandling(int index);

 private:
  QString path =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QSettings* settings =
      new QSettings(path + "/" + "settings.ini", QSettings::IniFormat);

  std::string prefix;
  void inputEvents();

  UINT32 HornerScheme(UINT32 Num, UINT32 Divider, UINT32 Factor);
  static void MyDispatchProcInput(SIMCONNECT_RECV* pData, DWORD cbData,
                                  void* pContext);
};

#endif  // INPUTWORKER_H