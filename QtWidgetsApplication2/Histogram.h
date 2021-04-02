#pragma once
#include <qvector.h>

class Histogram
{
private:
	QVector<int> redDistribution = QVector<int>(256);
	QVector<int> greenDistribution = QVector<int>(256);
	QVector<int> blueDistribution = QVector<int>(256);

public:
	Histogram(std::vector<uint8_t>& data);

	//friend class JA_Projekt;
	friend class QtWidgetsApplication2;
};


//void CountPixelValues(uint8_t* img, uint32_t imgSize, uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer);

//void NormalizeValues(uint32_t* rBuffer, uint32_t* gBuffer, uint32_t* bBuffer);