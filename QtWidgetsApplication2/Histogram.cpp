#include "Histogram.h"

/*void CountPixelValues(uint8_t* img, uint32_t imgSize, uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer)
{

	for (uint32_t i = 0; i < imgSize; i += 3)
	{

		bBuffer[*(img + i)]++;
		gBuffer[*(img + i + 1)]++;
		rBuffer[*(img + i + 2)]++;

	}
}

void NormalizeValues(uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer)
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
}*/

/*void create_histogram(uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer, std::string filepath, std::string histogramTitle)
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


}*/