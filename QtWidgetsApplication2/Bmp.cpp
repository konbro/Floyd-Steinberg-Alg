#include "Bmp.h"
#include <fstream>
#include <limits>

BMP::BMP()
{
	//FileHeader = new BMPFileHeader;

}

BMP::~BMP()
{
	if (FileHeader != nullptr)
		delete FileHeader;

	if (InfoHeader != nullptr)
		delete InfoHeader;

	if (pixelarray != nullptr)
		delete[] pixelarray;

	if (gap1 != nullptr)
		delete gap1;

	if (gap2 != nullptr)
		delete gap2;

}

int32_t BMP::calcPadding()
{
	int x = imgStride - imgWidth * BPP;
	return x;
}

int32_t BMP::getImgByteSize()
{
	return imgHeight * imgWidth * BPP;
}

uint64_t GetFileSize(std::string filepath)
{
	struct stat buff;
	int rc = stat(filepath.c_str(), &buff);

	if (rc == 0)
		return buff.st_size;
	else
		return -1;

}

void BMP::loadBMP(std::string filepath)
{
	try
	{
	auto fileSize = GetFileSize(filepath);
	if (fileSize < sizeof(BMPFileHeader) + 3 * sizeof(int32_t))
		throw std::exception("This file is to small to open!");

	if (fileSize > ((uint64_t)std::numeric_limits<uint32_t>::max()))
		throw std::exception("This file is to big to open!");


	
		std::ifstream file(filepath, std::ios::binary);
	

	if (file)
	{

		FileHeader = new BMPFileHeader;

		//Reading Fileheader and 2nd header size 
		file.read((char*)this->FileHeader, sizeof(BMPFileHeader));
		file.read((char*)&(this->dIBHeaderSize), sizeof(int32_t));


		if (dIBHeaderSize == sizeof(BMPInfoHeader))
		{
			this->InfoHeader = new BMPInfoHeader;

			//returning to posision after file header
			file.seekg(sizeof(BMPFileHeader), file.beg);
			file.read((char*)this->InfoHeader, sizeof(BMPInfoHeader));

			if (InfoHeader->bit_count != 24)
				throw std::exception("This program process only 24bpp bitmaps!");

			if (InfoHeader->compression != 0)
				throw std::exception("Only uncompressed files!");

			imgHeight = InfoHeader->height;
			imgWidth = InfoHeader->width;

		}
		else
		{
			file.read((char*)&(this->imgWidth), sizeof(int32_t));
			file.read((char*)&(this->imgHeight), sizeof(int32_t));

			ndHeaderPart = new uint8_t[dIBHeaderSize - 3 * sizeof(int32_t)];//3 from img 
			file.read((char*)ndHeaderPart, dIBHeaderSize - 3 * sizeof(int32_t));

		}

		imgStride = calcStride(imgWidth);
		imgPadding = calcPadding();

		gap1Size = FileHeader->offset_data - dIBHeaderSize - sizeof(BMPFileHeader);

		if (gap1Size > 0)
		{
			gap1 = new uint8_t[gap1Size];
			file.read((char*)gap1, gap1Size);
		}

		pixelarray = new uint8_t[getImgByteSize()];

		if (imgWidth % 4 == 0)
		{
			file.read((char*)pixelarray, getImgByteSize());
		}
		else
		{
			char padding[3] = { 0,0,0 };
			for (int i = 0; i < imgHeight; i++)
			{
				file.read((char*)pixelarray + i * imgWidth * BPP, imgWidth * BPP);
				file.read((char*)padding, imgPadding);
			}
		}


		gap2Size = this->FileHeader->file_size - sizeof(BMPFileHeader) - dIBHeaderSize - gap1Size - imgHeight * imgStride;

		if (gap2Size > 0)
		{
			gap2 = new uint8_t[gap2Size];
			file.read((char*)gap2, gap2Size);
		}
		file.close();
	}
	else
		throw (std::exception("File couldnt be opened!"));

	isLoaded = true;
	}
	catch (std::exception ex)
	{

	}

}


void BMP::saveBMP(std::string filepath)
{
	std::ofstream file(filepath, std::ios::binary);


	if (FileHeader == nullptr)
		throw std::exception("file header is corupted");

	file.write((char*)FileHeader, sizeof(BMPFileHeader));

	if (ndHeaderPart == nullptr)
	{
		file.write((char*)InfoHeader, sizeof(BMPInfoHeader));
	}
	else
	{
		file.write((char*)&(dIBHeaderSize), sizeof(uint32_t));
		file.write((char*)&(imgWidth), sizeof(imgWidth));
		file.write((char*)&(imgHeight), sizeof(imgHeight));
		file.write((char*)ndHeaderPart, dIBHeaderSize - 3 * sizeof(uint32_t));
	}

	if (gap1 != nullptr)
		file.write((char*)gap1, gap1Size);


	//writing pixel array

	char paddingTable[3] = { 0,0,0 };

	if (imgPadding == 0)
	{
		file.write((char*)pixelarray, getImgByteSize());
	}
	else
	{
		for (int i = 0; i < imgHeight; i++)
		{

			file.write((char*)pixelarray + imgWidth * BPP * i, BPP * imgWidth);
			file.write((char*)paddingTable, imgPadding);

		}
	}

	if (gap2 != nullptr)
		file.write((char*)gap2, gap2Size);

	file.close();
}

//calculates stride = width in bytes rounded to 4
int32_t BMP::calcStride(int32_t width)
{
	if (width % 4 == 0)
		return width * BPP;

	return 4 - ((width * BPP) % 4) + width * BPP;

}

uint8_t* BMP::getImg_No_Pad()
{
	return pixelarray;
}

int32_t BMP::getWidth()
{
	return this->imgWidth;
}

int32_t BMP::getHeight()
{
	return this->imgHeight;
}

void BMP::set_bmp_size(BMP* bmp_to_copy_from)
{
	this->set_Height(bmp_to_copy_from->getHeight());
	this->set_Width(bmp_to_copy_from->getWidth());
}

bool BMP::isBmpLoaded()
{
	return isLoaded;
}

void BMP::set_Width(int32_t new_width)
{
	this->imgWidth = new_width;
}

void BMP::set_Height(int32_t new_height)
{
	this->imgHeight = new_height;
}



