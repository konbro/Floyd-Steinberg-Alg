#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QButtonGroup>
#include <QtCharts/qlineseries.h>
#include <QtCharts/qbarset.h>
#include <QtCharts/qstackedbarseries.h>
#include <QtCharts/qchart.h>
#include <QtCharts/qchartview.h>
#include <qslider.h>
#include "ui_QtWidgetsApplication2.h"
#include "Bmp.h"

using namespace QtCharts;

class QtWidgetsApplication2 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication2(QWidget *parent = Q_NULLPTR);
    void CountPixelValues(uint8_t* img, uint32_t imgSize, uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer);
    void NormalizeValues(uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer);
    void create_histogram(uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer, std::string filepath, std::string histogramTitle);
    //void dither_bmp(uint8_t* input_bmp, uint32_t imSize);
    //void dither_bmp(uint8_t* input_bmp, uint32_t imSize, uint8_t* output_bmp);
    void dither_bmp(uint8_t* input_bmp, uint8_t* output_bmp, uint8_t* output_bmp_bw, uint32_t imSize, uint32_t imWidth, uint32_t imHeight);
    void dither_bmp_asm(uint8_t* input_bmp, uint8_t* output_bmp, uint8_t* output_bmp_bw, uint32_t imSize, uint32_t imWidth, uint32_t imHeight);
    void TransformPixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t* output_bmp, uint32_t index);
    BMP bmp_to_modify;
    BMP bmp_output;
    BMP bmp_output_blackandwhite;
    int bw_threshold = 128;

private:
    Ui::QtWidgetsApplication2Class ui;
    //std::string filepath;
private slots:
    void on_pushButton_CloseApp_clicked();
    void on_pushButton_SelectFile_clicked();
    void on_pushButton_RunAlgorithm_clicked();
    void on_horizontalSlider_valueChanged();
    //void on_QCheckbox_Cpp_checked();

private:
    QString filepath;
};
