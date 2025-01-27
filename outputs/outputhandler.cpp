#include "outputhandler.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonParseError>

outputHandler::outputHandler() { readOutputs(); }
// const QJsonObject &json

void outputHandler::readOutputs() {
  QFile file_obj("outputs.json");
  file_obj.open(QIODevice::ReadOnly | QIODevice::Text);
  QByteArray json_bytes = file_obj.readAll();
  file_obj.close();

  QJsonParseError error;
  QJsonDocument json_doc = QJsonDocument::fromJson(json_bytes, &error);

  if (error.error != QJsonParseError::NoError) {
    qDebug().noquote() << json_bytes[0];
    qDebug() << "Parse error: " << error.errorString();
    for (int i = error.offset - 10; i < error.offset + 30; i++) {
      qDebug().noquote() << json_bytes[i];
    }
    return;
  }

  if (!json_doc.isObject()) {
    qDebug() << "Document is not an object";
    return;
  }

  QJsonObject outputJSON = json_doc.object();
  categoryStrings = outputJSON.keys();

  foreach (const QString &v, categoryStrings) {
    QList<Output> *outputCategory = new QList<Output>;
    QJsonArray array = outputJSON.value(v).toArray();
    foreach (const QJsonValue &v, array) {
      QJsonObject obj = v.toObject();

      int JSONid = obj.value("id").toInt();
      std::string JSONoutputName =
          obj.value("outputName").toString().toStdString();
      std::string JSONmetric = obj.value("metric").toString().toStdString();
      float JSONupdateEvery = obj.value("updateEvery").toDouble();
      int JSONdataType = obj.value("dataType").toInt();
      QString JSONcbText = obj.value("cbText").toString();
      int prefix = obj.value("prefix").toInt();
      int type = obj.value("type").toInt();

      Output *foundOutput =
          new Output(JSONid, JSONoutputName, JSONmetric, JSONupdateEvery,
                     JSONdataType, JSONcbText, prefix, type);
      outputCategory->append(*foundOutput);
      availableOutputs.insert(foundOutput->getId(), foundOutput);

      QJsonValue score = obj.value("updateEvery");
    }
    outputsCategorized.append(*outputCategory);
  }
  qDebug() << availableOutputs.size() << " outputs saved";
}
Output *outputHandler::findOutputById(int idToFind) {
  qDebug() << idToFind;
  return availableOutputs.value(idToFind);
}
