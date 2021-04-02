
#pragma once

#ifndef BMP_H
#define BMP_H
#include <cstdint>
#include <string>


const int BPP = 3;
#pragma pack(push, 1) //to avoid adding padding by compiler

struct BMPFileHeader {
	uint16_t file_type{ 0x4D42 };          // File type always 'BM' which is 0x4D42
	uint32_t file_size{ 0 };               // Size of the file (in bytes)
	uint16_t reserved1{ 0 };               // Reserved, always 0
	uint16_t reserved2{ 0 };               // Reserved, always 0
	uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)	
};

#pragma pack(pop)

#pragma pack(push, 1)

struct BMPInfoHeader {
	uint32_t size{ 0 };                      // Size of this header (in bytes)
	int32_t width{ 0 };                      // width of bitmap in pixels
	int32_t height{ 0 };                     // height of bitmap in pixels
	uint16_t planes{ 1 };                    // No. of planes for the target device, this is always 1
	uint16_t bit_count{ 0 };                 // No. of bits per pixel
	uint32_t compression{ 0 };               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
	uint32_t size_image{ 0 };                // 0 - for uncompressed images
	int32_t x_pixels_per_meter{ 0 };		 //view setting
	int32_t y_pixels_per_meter{ 0 };		 //view setting
	uint32_t colors_used{ 0 };               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
	uint32_t colors_important{ 0 };          // No. of colors used for displaying the bitmap. If 0 all colors are required
};

#pragma pack(pop)

class BMP {

public:
	BMPFileHeader* FileHeader{ nullptr };
	BMPInfoHeader* InfoHeader{ nullptr };

protected:
	uint32_t dIBHeaderSize{ 0 };
	uint8_t* ndHeaderPart{ nullptr };//used only if uncommon header type

	//uint8_t extrapadding[3] = {0,0,0};

	uint8_t* gap1{ nullptr };//
	int32_t gap1Size{ 0 };

	uint8_t* gap2{ nullptr };
	int32_t gap2Size{ 0 };

	//----image information
	int32_t imgStride{ 0 };// width of line in bytes rounded to 4
	int32_t imgWidth{ 0 };
	int32_t imgHeight{ 0 };

	uint8_t* pixelarray{ nullptr };

	int32_t imgPadding{ 0 };
	int32_t calcPadding();

	bool isLoaded = false;
public:
	BMP();
	~BMP();

	//size of image without padding

	void loadBMP(std::string filepath);
	void saveBMP(std::string filepath);

	int32_t calcStride(int32_t width);

	uint8_t* getImg_No_Pad();
	int32_t getImgByteSize();
	int32_t getWidth();
	int32_t getHeight();
	void set_bmp_size(BMP* bmp_to_copy_from);
	

	bool isBmpLoaded();

	//void writeBmp(std::string filename);

private:
	void set_Width(int32_t new_width);
	void set_Height(int32_t new_height);
};






#endif // !BMP_H

