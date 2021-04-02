#include "QtWidgetsApplication2.h"
#include "Histogram.h"
#include "Connector.h"


QtWidgetsApplication2::QtWidgetsApplication2(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void QtWidgetsApplication2::on_pushButton_CloseApp_clicked()
{
    close();
}

/*
* zmienianie wartoœci granicznej wg której zostan¹ przetworzone pliki graficzne
*/
void QtWidgetsApplication2::on_horizontalSlider_valueChanged()
{
    bw_threshold = ui.horizontalSlider->value();
    ui.label_slider_value->setText(QString::number(ui.horizontalSlider->value()));
}

/*
* metoda odpowiedzialna za uruchomienie okna s³u¿¹cego do wyboru pliku .bmp
*/
void QtWidgetsApplication2::on_pushButton_SelectFile_clicked()
{
    filepath = QFileDialog::getOpenFileName(this, tr("Please choose BMP file"), "", tr("Image Files (*.bmp)"));
    bmp_to_modify.loadBMP(filepath.toStdString());
    bmp_output.loadBMP(filepath.toStdString());
    bmp_output_blackandwhite.loadBMP(filepath.toStdString());
    ui.label_4->setText(filepath);

}

/*
* metoda uruchamiana przy kliknieciu w przycisk uruchamiaj¹cy algorytm
* w zale¿noœci od wybranej biblioteki DLL jest wybierana funkcja
*/
void QtWidgetsApplication2::on_pushButton_RunAlgorithm_clicked()
{
    uint32_t r[256] = { 0 }, g[256] = { 0 }, b[256] = { 0 };
    this->CountPixelValues(this->bmp_to_modify.getImg_No_Pad(), this->bmp_to_modify.getImgByteSize(), r, g, b);
    this->NormalizeValues(r, g, b);
    this->create_histogram(r, g, b, "output//input_histogram", "Histogram");
    if (ui.radioButton_cpp->isChecked())
    {
        this->dither_bmp(bmp_to_modify.getImg_No_Pad(), bmp_output.getImg_No_Pad(), bmp_output_blackandwhite.getImg_No_Pad(), bmp_to_modify.getImgByteSize(), bmp_to_modify.getWidth(), bmp_to_modify.getHeight());
        bmp_output.saveBMP("output//output_cpp_" + std::to_string(bw_threshold) + "_floyd.bmp");
        bmp_output_blackandwhite.saveBMP("output//output_cpp_" + std::to_string(bw_threshold) + "_bw.bmp");
        ui.label_error_label->setText("C++ PROCESS FINISHED FOR B/W THRESHOLD: " + QString::number(this->bw_threshold));
    }
    else if (ui.radioButton_asm->isChecked())
    {
        this->dither_bmp_asm(bmp_to_modify.getImg_No_Pad(), bmp_output.getImg_No_Pad(), bmp_output_blackandwhite.getImg_No_Pad(), bmp_to_modify.getImgByteSize(), bmp_to_modify.getWidth(), bmp_to_modify.getHeight());
        bmp_output.saveBMP("output//output_asm_" + std::to_string(bw_threshold) + "_floyd.bmp");
        bmp_output_blackandwhite.saveBMP("output//output_asm_" + std::to_string(bw_threshold) + "_bw.bmp");
        ui.label_error_label->setText("ASM PROCESS FINISHED FOR B/W THRESHOLD: " + QString::number(this->bw_threshold));
    }
    else
    {
        ui.label_error_label->setText("NO ALGORITHM TYPE SELECTED");
    }
    /*bmp_output.saveBMP("output//output_" + std::to_string(bw_threshold) + "_floyd.bmp");
    bmp_output_blackandwhite.saveBMP("output//output_" + std::to_string(bw_threshold) + "_bw.bmp");*/
}

QButtonGroup::QButtonGroup(QObject* parent)
{

}

/*
* metoda sluzaca do zliczenia wartosci posczegolnych czesci skladowej RGB
*/
void QtWidgetsApplication2::CountPixelValues(uint8_t* img, uint32_t imgSize, uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer)
{

    for (uint32_t i = 0; i < imgSize; i += 3)
    {

        bBuffer[*(img + i)]++;
        gBuffer[*(img + i + 1)]++;
        rBuffer[*(img + i + 2)]++;

    }
}

/*
Normalizacja wartoœci które zostan¹ nastêpnie wykorzystane w tworzeniu histogramu
*/
void QtWidgetsApplication2::NormalizeValues(uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer)
{
    constexpr uint32_t strecher = 100;
    uint32_t max = 1;

    for (int i = 0; i < 256; i++)
    {

        max = std::max(max, rBuffer[i]);
        max = std::max(max, gBuffer[i]);
        max = std::max(max, bBuffer[i]);

    }

    for (int i = 0; i < 256; i++)
    {
        bBuffer[i] = bBuffer[i] * strecher / max;
        gBuffer[i] = gBuffer[i] * strecher / max;
        rBuffer[i] = rBuffer[i] * strecher / max;
    }
}

/*
* Tworzenie histogramu RGB oraz zapisywanie go do folderu output
*/
void QtWidgetsApplication2::create_histogram(uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer, std::string filepath, std::string histogramTitle)
{
    QtCharts::QLineSeries* rSeries = new QtCharts::QLineSeries();
    QtCharts::QLineSeries* gSeries = new QtCharts::QLineSeries();
    QtCharts::QLineSeries* bSeries = new QtCharts::QLineSeries();


    for (int i = 0; i < 256; i++)
    {
        *rSeries << QPoint(i, rBuffer[i]);
        *gSeries << QPoint(i, gBuffer[i]);
        *bSeries << QPoint(i, bBuffer[i]);
    }

    rSeries->setColor(Qt::red);
    gSeries->setColor(Qt::green);
    bSeries->setColor(Qt::blue);

    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->addSeries(rSeries);
    chart->addSeries(gSeries);
    chart->addSeries(bSeries);
    chart->createDefaultAxes();
    chart->setTitle(QString::fromStdString(histogramTitle));

    QtCharts::QChartView* chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    QPixmap p = chartView->grab();

    p.save(QString::fromStdString(filepath + ".png"), "PNG");

}

void QtWidgetsApplication2::dither_bmp(uint8_t* input_bmp, uint8_t* output_bmp, uint8_t* output_bmp_bw, uint32_t imSize, uint32_t imWidth, uint32_t imHeight)
{
    //for (uint32_t i = 0; i < imSize; i += 3)
    /*for (uint32_t i = 0; i < imSize/3 - 250; i += 3)
    {
        uint8_t avg = ((uint16_t)input_bmp[i] + (uint16_t)input_bmp[i + 1] + (uint16_t)input_bmp[i + 2]) / 3;
        output_bmp[i] = 255; //b pixel
        output_bmp[i + 1] = avg; // g pixel
        output_bmp[i + 2] = avg; //r pixel
    }*/

    //imSize is a pixel array, there should be an option to divide into "lines" inorder to be able to use floyd steinberd
    //NOTE pixel array is 3* bigger than normal count of pixels in img due to division in R G B
    // ALSO NOTE THAT IT WORKS FROM BOTTOM TO TOP


    ////////////////////////////////////////////////////////////
    //for (uint32_t i = imSize; i > imSize / 2; i -= 3)
    //for (uint32_t i = 0; i < imSize / 3; i++)
    for (uint32_t i = 0; i < imSize; i+=3)
    {
        //TransformPixel(input_bmp[i * 3 + 2], input_bmp[i * 3 + 1], input_bmp[i * 3], output_bmp_bw, i * 3);
        TransformPixel(input_bmp[i + 2], input_bmp[i + 1], input_bmp[i], output_bmp_bw, i);
    }
    //for (uint32_t i = imSize - 3; i > 0; i -= 3)
    //for (uint32_t i = (imSize / 2) - 3; i > 0; i -= 3)
    //for (uint32_t i = 0; i < imSize/2; i += 3)
    //{
    //    //X
    //    /*uint8_t avg = ((uint16_t)input_bmp[i] + (uint16_t)input_bmp[i + 1] + (uint16_t)input_bmp[i + 2]) / 3;
    //    output_bmp[i] = 255; //b pixel
    //    output_bmp[i + 1] = avg; // g pixel
    //    output_bmp[i + 2] = avg; //r pixel*/
    //    /*
    //              x   7/16
    //        3/16 5/16 1/16
    //    
    //    */

    //    //7/16
    //    // next pixel so +3
    //    //what if reached end of line? 
    //    //uint8_t avg_7 = ((uint16_t)input_bmp[i + 3] + (uint16_t)input_bmp[i + 3 + 1] + (uint16_t)input_bmp[i + 3 + 2]) / 3;
    //    /*uint8_t redError_7 = input_bmp[i + 2] - input_bmp[i + 2 + 3]; //red error
    //    uint8_t blueError_7 = input_bmp[i] - input_bmp[i + 3]; //blue error
    //    uint8_t greenError_7 = input_bmp[i + 1] - input_bmp[i + 1 + 3]; //green error*/
    //    uint8_t redError = input_bmp[i + 2] - output_bmp_bw[i + 2]; //red error
    //    uint8_t greenError = input_bmp[i + 1] - output_bmp_bw[i + 1]; //green error
    //    uint8_t blueError = input_bmp[i] - output_bmp_bw[i];

    //    //pixel on the right
    //    //if(!(i/3)%imWidth==imWidth-1)
    //    if (((i / 3) % imWidth )!= imWidth - 1)
    //    {
    //        /*
    //        OPTION A
    //        */
    //        //blue val of pixel
    //        output_bmp[i] = input_bmp[i + 3] + redError * 7/16;
    //        //green val of pixel
    //        output_bmp[i + 1] = input_bmp[i + 1 + 3] + greenError * 7 / 16;
    //        //red val of pixel
    //        output_bmp[i + 2] = input_bmp[i + 2 + 3] + blueError * 7 / 16;
    //        /*
    //        OPTION B
    //        BINARY SHIFT
    //        */
    //    }
    //    //pixels below current pixel
    //    if (i < imSize - imHeight * 3)
    //    {
    //        //left and down
    //        if (!(i / 3) % imHeight == 0)
    //        {
    //            /*
    //            OPTION A
    //            */
    //            //blue val of pixel
    //            output_bmp[i] = input_bmp[i - 3 + imWidth*3 ] + redError * 3 / 16;
    //            //green val of pixel
    //            output_bmp[i + 1] = input_bmp[i + 1 - 3 + imWidth * 3] + greenError * 3 / 16;
    //            //red val of pixel
    //            output_bmp[i + 2] = input_bmp[i + 2 - 3 + imWidth * 3] + blueError * 3 / 16;
    //        }
    //        //down
    //        /*if ()
    //        {

    //        }*/
    //            /*
    //            OPTION A
    //            */
    //        //blue val of pixel
    //        output_bmp[i] = input_bmp[i + imWidth * 3] + redError * 5 / 16;
    //        //green val of pixel
    //        output_bmp[i + 1] = input_bmp[i + 1 + imWidth * 3] + greenError * 5 / 16;
    //        //red val of pixel
    //        output_bmp[i + 2] = input_bmp[i + 2 + imWidth * 3] + blueError * 5 / 16;
    //        //down and right
    //        if (!(i / 3) % imWidth == imWidth - 1)
    //        {
    //            /*
    //           OPTION A
    //           */
    //           //blue val of pixel
    //            output_bmp[i] = input_bmp[i + 3 + imWidth * 3] + redError * 1 / 16;
    //            //green val of pixel
    //            output_bmp[i + 1] = input_bmp[i + 1 + 3 + imWidth * 3] + greenError * 1 / 16;
    //            //red val of pixel
    //            output_bmp[i + 2] = input_bmp[i + 2 + 3 + imWidth * 3] + blueError * 1 / 16;
    //        }
    //    }
    //    //output_bmp[i + 3] = 0;

    //    //3/16
    //    // +imWidth - 3
    //    //uint8_t avg_3 = ((uint16_t)input_bmp[i + imWidth - 3] + (uint16_t)input_bmp[i + imWidth - 2] + (uint16_t)input_bmp[i + imWidth - 1]) / 3;
    //    //5/16
    //    //pixel below so + imWidth
    //    //make if to make sure to not access unreachable memory
    //    //uint8_t avg_5 = ((uint16_t)input_bmp[i + imWidth] + (uint16_t)input_bmp[i + imWidth + 1] + (uint16_t)input_bmp[i + imWidth + 2]) / 3;

    //    //1/16
    //    // +imWidth+3
    //    //uint8_t avg_1 = ((uint16_t)input_bmp[i + imWidth + 3] + (uint16_t)input_bmp[i + imWidth + 4] + (uint16_t)input_bmp[i + imWidth + 5]) / 3;
    //    
    //}
    for (uint32_t i = 0; i < imSize; i += 3)
    {
        int32_t redError = input_bmp[i + 2] - output_bmp_bw[i + 2]; //red error
        int32_t greenError = input_bmp[i + 1] - output_bmp_bw[i + 1]; //green error
        int32_t blueError = input_bmp[i] - output_bmp_bw[i];

        ///////////////////
        //input_bmp = output_bmp_bw;
        //////////////////////
        //pixel on the right
        //if(!(i/3)%imWidth==imWidth-1)
        if (((i / 3) % imWidth) != imWidth - 1)
        {
            /*
            OPTION A
            */
            //blue val of pixel
            /*uint8_t test_b = (input_bmp[i + 3]) + (redError * 7 / 16);
            uint8_t test_g = input_bmp[i + 1 + 3] + greenError * 7 / 16;
            uint8_t test_r = input_bmp[i + 2 + 3] + blueError * 7 / 16;*/
            output_bmp[i + 3] = input_bmp[i + 3] + blueError * 7 / 16;
            if ((input_bmp[i + 3] + blueError * 7 / 16) > 255)
            {
                output_bmp[i + 3] = 255;
            }
            else if ((input_bmp[i + 3] + blueError * 7 / 16) < 0)
            {
                output_bmp[i + 3] = 0;
            }
            //green val of pixel
            output_bmp[i + 1 + 3] = input_bmp[i + 1 + 3] + greenError * 7 / 16;
            if ((input_bmp[i + 1 + 3] + greenError * 7 / 16) > 255)
            {
                output_bmp[i + 1 + 3] = 255;
            }
            else if ((input_bmp[i + 1 + 3] + greenError * 7 / 16) < 0)
            {
                output_bmp[i + 1 + 3] = 0;
            }
            //red val of pixel
            output_bmp[i + 2 + 3] = input_bmp[i + 2 + 3] + redError * 7 / 16;
            if ((input_bmp[i + 2 + 3] + redError * 7 / 16) > 255)
            {
                output_bmp[i + 2 + 3] = 255;
            }
            else if ((input_bmp[i + 2 + 3] + redError * 7 / 16) < 0)
            {
                output_bmp[i + 2 + 3] = 0;
            }
            /*
            OPTION B
            BINARY SHIFT
            */
        }
        //pixels below current pixel
        //if (i > imHeight * 3)
        if (i > imWidth * 3)
        {
            //left and down
            if ((i / 3) % imHeight != 0)
            //if ((i / 3) % imWidth != 0)
            {
                /*
                OPTION A
                */
                //blue val of pixel
                output_bmp[i - 3 - imWidth * 3] = input_bmp[i - 3 - imWidth * 3] + (blueError * 3 / 16);
                if ((input_bmp[i - 3 - imWidth * 3] + (blueError * 3 / 16)) > 255)
                {
                    output_bmp[i - 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i - 3 - imWidth * 3] + (blueError * 3 / 16)) < 0)
                {
                    output_bmp[i - 3 - imWidth * 3] = 0;
                }
                //green val of pixel
                output_bmp[i + 1 - 3 - imWidth * 3] = input_bmp[i + 1 - 3 - imWidth * 3] + (greenError * 3 / 16);
                if ((input_bmp[i + 1 - 3 - imWidth * 3] + (greenError * 3 / 16)) > 255)
                {
                    output_bmp[i + 1 - 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 1 - 3 - imWidth * 3] + (greenError * 3 / 16)) <0)
                {
                    output_bmp[i + 1 - 3 - imWidth * 3] = 0;
                }
                //red val of pixel
                output_bmp[i + 2 - 3 - imWidth * 3] = input_bmp[i + 2 - 3 - imWidth * 3] + (redError * 3 / 16);
                if ((input_bmp[i + 2 - 3 - imWidth * 3] + (redError * 3 / 16)) > 255)
                {
                    output_bmp[i + 2 - 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 2 - 3 - imWidth * 3] + (redError * 3 / 16)) < 0)
                {
                    output_bmp[i + 2 - 3 - imWidth * 3] = 0;
                }
            }
            //down
            /*if ()
            {

            }*/
            /*
            OPTION A
            */
            //blue val of pixel
            output_bmp[i - imWidth * 3] = input_bmp[i - imWidth * 3] + (blueError * 5 / 16);
            if ((input_bmp[i - imWidth * 3] + (blueError * 5 / 16)) > 255)
            {
                output_bmp[i - imWidth * 3] = 255;
            }
            else if ((input_bmp[i - imWidth * 3] + (blueError * 5 / 16)) < 0)
            {
                output_bmp[i - imWidth * 3] = 0;
            }
            //green val of pixel
            output_bmp[i + 1 - imWidth * 3] = input_bmp[i + 1 - imWidth * 3] + (greenError * 5 / 16);
            if ((input_bmp[i + 1 - imWidth * 3] + (greenError * 5 / 16)) > 255)
            {
                output_bmp[i + 1 - imWidth * 3] = 255;
            }
            else if ((input_bmp[i + 1 - imWidth * 3] + (greenError * 5 / 16)) < 0)
            {
                output_bmp[i + 1 - imWidth * 3] = 0;
            }
            //red val of pixel
            output_bmp[i + 2 - imWidth * 3] = input_bmp[i + 2 - imWidth * 3] + (redError * 5 / 16);
            if ((input_bmp[i + 2 - imWidth * 3] + (redError * 5 / 16)) > 255)
            {
                output_bmp[i + 2 - imWidth * 3] = 255;
            }
            else if ((input_bmp[i + 2 - imWidth * 3] + (redError * 5 / 16)) <0)
            {
                output_bmp[i + 2 - imWidth * 3] = 0;
            }
            //down and right
            if (!(i / 3) % imWidth == imWidth - 1)
            {
                /*
               OPTION A
               */
               //blue val of pixel
                output_bmp[i + 3 - imWidth * 3] = input_bmp[i + 3 - imWidth * 3] + (blueError * 1 / 16);
                if ((input_bmp[i + 3 - imWidth * 3] + (blueError * 1 / 16)) > 255)
                {
                    output_bmp[i + 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 3 - imWidth * 3] + (blueError * 1 / 16)) < 0)
                {
                    output_bmp[i + 3 - imWidth * 3] = 0;
                }
                //green val of pixel
                output_bmp[i + 1 + 3 - imWidth * 3] = input_bmp[i + 1 + 3 - imWidth * 3] + (greenError * 1 / 16);
                if ((input_bmp[i + 1 + 3 - imWidth * 3] + (greenError * 1 / 16)) > 255)
                {
                    output_bmp[i + 1 + 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 1 + 3 - imWidth * 3] + (greenError * 1 / 16)) < 0)
                {
                    output_bmp[i + 1 + 3 - imWidth * 3] = 0;
                }
                //red val of pixel
                output_bmp[i + 2 + 3 - imWidth * 3] = input_bmp[i + 2 + 3 - imWidth * 3] + (redError * 1 / 16);
                if ((input_bmp[i + 2 + 3 - imWidth * 3] + (redError * 1 / 16)) > 255)
                {
                    output_bmp[i + 2 + 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 2 + 3 - imWidth * 3] + (redError * 1 / 16)) < 0)
                {
                    output_bmp[i + 2 + 3 - imWidth * 3] = 0;
                }
            }
        }
    

}
}

void QtWidgetsApplication2::dither_bmp_asm(uint8_t* input_bmp, uint8_t* output_bmp, uint8_t* output_bmp_bw, uint32_t imSize, uint32_t imWidth, uint32_t imHeight)
{
    for (uint32_t i = 0; i < imSize; i += 3)
    {
        uint8_t gray = 0.299 * input_bmp[i + 2] + 0.587 * input_bmp[i + 1] + 0.114 * input_bmp[i];
        ez_connect connection_to_asm = ez_connect();
        connection_to_asm.loadDLLAsm();
        if(connection_to_asm.TPAF(gray, this->bw_threshold)==0)
        {
            output_bmp_bw[i] = 255;
            output_bmp_bw[i + 1] = 255;
            output_bmp_bw[i + 2] = 255;
        }
        else
        {
            output_bmp_bw[i] = 0;
            output_bmp_bw[i + 1] = 0;
            output_bmp_bw[i + 2] = 0;
        }
    }
   
    for (uint32_t i = 0; i < imSize; i += 3)
    {
        int32_t redError = input_bmp[i + 2] - output_bmp_bw[i + 2]; //red error
        int32_t greenError = input_bmp[i + 1] - output_bmp_bw[i + 1]; //green error
        int32_t blueError = input_bmp[i] - output_bmp_bw[i];

        ///////////////////
        //input_bmp = output_bmp_bw;
        //////////////////////
        //pixel on the right
        //if(!(i/3)%imWidth==imWidth-1)
        if (((i / 3) % imWidth) != imWidth - 1)
        {
            /*
            OPTION A
            */
            //blue val of pixel
            /*uint8_t test_b = (input_bmp[i + 3]) + (redError * 7 / 16);
            uint8_t test_g = input_bmp[i + 1 + 3] + greenError * 7 / 16;
            uint8_t test_r = input_bmp[i + 2 + 3] + blueError * 7 / 16;*/
            output_bmp[i + 3] = input_bmp[i + 3] + blueError * 7 / 16;
            if ((input_bmp[i + 3] + blueError * 7 / 16) > 255)
            {
                output_bmp[i + 3] = 255;
            }
            else if ((input_bmp[i + 3] + blueError * 7 / 16) < 0)
            {
                output_bmp[i + 3] = 0;
            }
            //green val of pixel
            output_bmp[i + 1 + 3] = input_bmp[i + 1 + 3] + greenError * 7 / 16;
            if ((input_bmp[i + 1 + 3] + greenError * 7 / 16) > 255)
            {
                output_bmp[i + 1 + 3] = 255;
            }
            else if ((input_bmp[i + 1 + 3] + greenError * 7 / 16) < 0)
            {
                output_bmp[i + 1 + 3] = 0;
            }
            //red val of pixel
            output_bmp[i + 2 + 3] = input_bmp[i + 2 + 3] + redError * 7 / 16;
            if ((input_bmp[i + 2 + 3] + redError * 7 / 16) > 255)
            {
                output_bmp[i + 2 + 3] = 255;
            }
            else if ((input_bmp[i + 2 + 3] + redError * 7 / 16) < 0)
            {
                output_bmp[i + 2 + 3] = 0;
            }
            /*
            OPTION B
            BINARY SHIFT
            */
        }
        //pixels below current pixel
        //if (i > imHeight * 3)
        if (i > imWidth * 3)
        {
            //left and down
            if ((i / 3) % imHeight != 0)
                //if ((i / 3) % imWidth != 0)
            {
                /*
                OPTION A
                */
                //blue val of pixel
                output_bmp[i - 3 - imWidth * 3] = input_bmp[i - 3 - imWidth * 3] + (blueError * 3 / 16);
                if ((input_bmp[i - 3 - imWidth * 3] + (blueError * 3 / 16)) > 255)
                {
                    output_bmp[i - 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i - 3 - imWidth * 3] + (blueError * 3 / 16)) < 0)
                {
                    output_bmp[i - 3 - imWidth * 3] = 0;
                }
                //green val of pixel
                output_bmp[i + 1 - 3 - imWidth * 3] = input_bmp[i + 1 - 3 - imWidth * 3] + (greenError * 3 / 16);
                if ((input_bmp[i + 1 - 3 - imWidth * 3] + (greenError * 3 / 16)) > 255)
                {
                    output_bmp[i + 1 - 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 1 - 3 - imWidth * 3] + (greenError * 3 / 16)) < 0)
                {
                    output_bmp[i + 1 - 3 - imWidth * 3] = 0;
                }
                //red val of pixel
                output_bmp[i + 2 - 3 - imWidth * 3] = input_bmp[i + 2 - 3 - imWidth * 3] + (redError * 3 / 16);
                if ((input_bmp[i + 2 - 3 - imWidth * 3] + (redError * 3 / 16)) > 255)
                {
                    output_bmp[i + 2 - 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 2 - 3 - imWidth * 3] + (redError * 3 / 16)) < 0)
                {
                    output_bmp[i + 2 - 3 - imWidth * 3] = 0;
                }
            }
            //down
            /*if ()
            {

            }*/
            /*
            OPTION A
            */
            //blue val of pixel
            output_bmp[i - imWidth * 3] = input_bmp[i - imWidth * 3] + (blueError * 5 / 16);
            if ((input_bmp[i - imWidth * 3] + (blueError * 5 / 16)) > 255)
            {
                output_bmp[i - imWidth * 3] = 255;
            }
            else if ((input_bmp[i - imWidth * 3] + (blueError * 5 / 16)) < 0)
            {
                output_bmp[i - imWidth * 3] = 0;
            }
            //green val of pixel
            output_bmp[i + 1 - imWidth * 3] = input_bmp[i + 1 - imWidth * 3] + (greenError * 5 / 16);
            if ((input_bmp[i + 1 - imWidth * 3] + (greenError * 5 / 16)) > 255)
            {
                output_bmp[i + 1 - imWidth * 3] = 255;
            }
            else if ((input_bmp[i + 1 - imWidth * 3] + (greenError * 5 / 16)) < 0)
            {
                output_bmp[i + 1 - imWidth * 3] = 0;
            }
            //red val of pixel
            output_bmp[i + 2 - imWidth * 3] = input_bmp[i + 2 - imWidth * 3] + (redError * 5 / 16);
            if ((input_bmp[i + 2 - imWidth * 3] + (redError * 5 / 16)) > 255)
            {
                output_bmp[i + 2 - imWidth * 3] = 255;
            }
            else if ((input_bmp[i + 2 - imWidth * 3] + (redError * 5 / 16)) < 0)
            {
                output_bmp[i + 2 - imWidth * 3] = 0;
            }
            //down and right
            if (!(i / 3) % imWidth == imWidth - 1)
            {
                /*
               OPTION A
               */
               //blue val of pixel
                output_bmp[i + 3 - imWidth * 3] = input_bmp[i + 3 - imWidth * 3] + (blueError * 1 / 16);
                if ((input_bmp[i + 3 - imWidth * 3] + (blueError * 1 / 16)) > 255)
                {
                    output_bmp[i + 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 3 - imWidth * 3] + (blueError * 1 / 16)) < 0)
                {
                    output_bmp[i + 3 - imWidth * 3] = 0;
                }
                //green val of pixel
                output_bmp[i + 1 + 3 - imWidth * 3] = input_bmp[i + 1 + 3 - imWidth * 3] + (greenError * 1 / 16);
                if ((input_bmp[i + 1 + 3 - imWidth * 3] + (greenError * 1 / 16)) > 255)
                {
                    output_bmp[i + 1 + 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 1 + 3 - imWidth * 3] + (greenError * 1 / 16)) < 0)
                {
                    output_bmp[i + 1 + 3 - imWidth * 3] = 0;
                }
                //red val of pixel
                output_bmp[i + 2 + 3 - imWidth * 3] = input_bmp[i + 2 + 3 - imWidth * 3] + (redError * 1 / 16);
                if ((input_bmp[i + 2 + 3 - imWidth * 3] + (redError * 1 / 16)) > 255)
                {
                    output_bmp[i + 2 + 3 - imWidth * 3] = 255;
                }
                else if ((input_bmp[i + 2 + 3 - imWidth * 3] + (redError * 1 / 16)) < 0)
                {
                    output_bmp[i + 2 + 3 - imWidth * 3] = 0;
                }
            }
        }


    }
}

void QtWidgetsApplication2::TransformPixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t* output_bmp, uint32_t index)
{
    uint8_t gray = 0.299 * red + 0.587 * green + 0.114 * blue;
    if (gray < bw_threshold)
    {
        output_bmp[index] = 0;      //b
        output_bmp[index + 1] = 0;  //g
        output_bmp[index + 2] = 0;  //r
    }
    else
    {
        output_bmp[index] = 255;      //b
        output_bmp[index + 1] = 255;  //g
        output_bmp[index + 2] = 255;  //r
    }
}
