#ifndef FORMBUILDER_H
#define FORMBUILDER_H

#include <outputs/outputhandler.h>
#include <outputs/set.h>
#include <outputs/sethandler.h>
#include "coordinates.h"
#include <qstring.h>
#include <qtabwidget.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QtCharts>
#include <qcustomplot.h>
#include "settingshandler.h"

class FormBuilder : public QObject {
Q_OBJECT

public:
    FormBuilder();
    //~FormBuilder();

    QVBoxLayout *RangeBuilder();

    void loadComPortData();

    QVBoxLayout *generateComColumn(int index);

    QHBoxLayout *generateComBlock();

    static QVBoxLayout *generateRange(const QString &header);

    QVBoxLayout *createRudderRow();

    QStringList comHeaders = {"Inputs", "Outputs", "Both"};
    QStringList rangeHeaders;
    QStringList engineLabels = {"Reverse", "Idle cutoff", "Max"};
    QStringList engineHeaders;

    int getAmountOfEngines() { return engineHeaders.size(); };

    static QTabWidget *generateOutputTabs();

    static QVBoxLayout *generateOutputSetList();

    QGridLayout *generateOutputControls() const;
    // QVBoxLayout *generateActiveSet();

    QWidget *generateSetRow(const set &setForRow);

    QHBoxLayout *generateOutputRow(Output *output);

    static QWidget *generateActiveSet(set *selectedSet);

    static QLabel *generateHeader(const QString &text);

    QWidget *generateComSelector(bool setsNeeded, int mode);

    QWidget *generateComControls(int mode);

    QList<set> *getAvailableSets() { return availableSets; };

    QList<QString> getAvailableComPorts() { return availableComPorts; };

    QStringList getRudderCalibrateLabels() { return rudderObjectNames; };

    QList<struct coordinates> *getCoordinates();

private slots:

    void localRemove();

    void localEdit();

    void localStart();

    void localRefreshed();

    void localStop();

    void localAdd();

    void removeComWidget();

    void rudderTextChanged();

    void updateX();

    void reverseClicked();

    void updateY();

signals:

    void addSet();

    void setEdited(QString id);

    void removeSet(QString id);

    void startPressed(int mode);

    void refreshPressed(int mode);

    void stopPressed(int mode);

    void addPressed(int mode);

private:
    QStringList rudderObjectNames = {"rudderMinLE", "rudderNeutralLE",
                                     "rudderMaxLE"};
    QList<int> axisValues = {-16383,-10000,0,0,0,10000,16383};
    QChart *chart;
    QCustomPlot *cplot;
    QLineSeries *series;
    QChartView *chartView;
    QStringList mainHeaders = {"", "INPUT", "OUTPUT", "DUAL"};
    SetHandler setHandler;
    SettingsHandler settingsHandler;
    QList<set> *availableSets;
    QList<QString> availableComPorts;


    void updateChart();


    QList<coordinates> pointsToPlot;


    QVBoxLayout *generateCurveCol(int valAxis, int valRange);

    void changeSlider();

    int minRudderValue = 0;

    int maxRudderValue = 1023;

    int neutralRudderValue = (maxRudderValue - minRudderValue) / 2;


};

#endif  // FORMBUILDER_H
