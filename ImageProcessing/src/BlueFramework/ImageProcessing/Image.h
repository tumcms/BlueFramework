/*
    This file is part of BlueFramework, a simple 3D engine.
	Copyright (c) 2016-2017 Technical University of Munich
	Chair of Computational Modeling and Simulation.

    BlueFramework is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    BlueFramework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#ifndef BlueFramework_ImageProcessing_Color_f5371cca_cd2f_43c7_b795_35427ef324dc_h
#define BlueFramework_ImageProcessing_Color_f5371cca_cd2f_43c7_b795_35427ef324dc_h

#include "BlueFramework/ImageProcessing/Color.h"
#include "BlueFramework/ImageProcessing/namespace.h"
#include "BlueFramework/Core/assert.h"

BLUEFRAMEWORK_IMAGEPROCESSING_NAMESPACE_BEGIN

template<typename ColorType>
class Image {
public:
	Image(const int width, const int height) :
	width_(width),
	height_(height) {
		data_ = new ColorType[width_ * height_];
	}

	Image(const Image& src) {
		width_ = src.width_;
		height_ = src.height_;
		int size = width_ * height_;
		data_ = new ColorType[size];

		for (int i = 0; i < size; ++i) {
			data_[i] = src.data_[i];
		}
	}
	
	virtual ~Image() {
		delete[] data_;
	}

	void resize(int width, int height)
	{
		if (data_)
			delete[] data_;
		width_ = width;
		height_ = height;
		data_ = new ColorType[width_ * height_];
	}

	int getWidth() const { return width_; }
	int getHeight() const { return height_; }
    const bool checkBounds(const int x, const int y) const { return x >= 0 && x < getWidth() && y >= 0 && y < getHeight(); }
    const int getRowPitch() const { return static_cast<int>(getElementByteSize() * getWidth()); }
    const int getDepthPitch() const { return getRowPitch() * getHeight(); }
    const size_t getElementByteSize() const { return sizeof(ColorType); }
	const ColorType* getData() const { return data_; }
    ColorType* getData() { return data_; }

	/*
		The following layout is used:
	    +-----+-----+-----+
	    |(0|0)|(1|0)|(2|0)|
	    +-----+-----+-----+
	    |(0|1)|(1|1)|(2|1)|
	    +-----+-----+-----+
	    |(0|2)|(1|2)|(2|2)|
	    +-----+-----+-----+
	*/
	void setPixelColor(const int x, const int y, const ColorType& color) {
        BLUE_ASSERT(checkBounds(x, y));
        data_[x + y*width_] = color;
	}

	/*
		The following layout is used:
		+-----+-----+-----+
		|(0|2)|(1|2)|(2|2)|
		+-----+-----+-----+
		|(0|1)|(1|1)|(2|1)|
		+-----+-----+-----+
		|(0|0)|(1|0)|(2|0)|
		+-----+-----+-----+
	*/
	void setPixelColorStandardCartesian(const int x, const int y, const ColorType& color) {
        BLUE_ASSERT(checkBounds(x, y));
        data_[x + (height_-y-1)*width_] = color;
	}

	/*
		The following layout is used:
		+-----+-----+-----+
		|(0|0)|(1|0)|(2|0)|
		+-----+-----+-----+
		|(0|1)|(1|1)|(2|1)|
		+-----+-----+-----+
		|(0|2)|(1|2)|(2|2)|
		+-----+-----+-----+
	*/
	ColorType getPixelColor(const int x, const int y) const {
        BLUE_ASSERT(checkBounds(x, y));
        return data_[x + y*width_];
	}



private:
	int width_;
	int height_;

	/*
		The following layout is used:
		+-----+-----+-----+
		|(0|0)|(1|0)|(2|0)|
		+-----+-----+-----+
		|(0|1)|(1|1)|(2|1)|
		+-----+-----+-----+
		|(0|2)|(1|2)|(2|2)|
		+-----+-----+-----+
	*/
	ColorType* data_;
};

typedef Image<Color3b> Image3b;
typedef Image<Color3f> Image3f;
typedef Image<Color3d> Image3d;

typedef Image<Color4b> Image4b;
typedef Image<Color4f> Image4f;
typedef Image<Color4d> Image4d;

BLUEFRAMEWORK_IMAGEPROCESSING_NAMESPACE_END

BLUE_BLUEFRAMEWORK_IMAGEPROCESSING_EMBED_INTO_BUW_NAMESPACE(Image)
BLUE_BLUEFRAMEWORK_IMAGEPROCESSING_EMBED_INTO_BUW_NAMESPACE(Image3b)
BLUE_BLUEFRAMEWORK_IMAGEPROCESSING_EMBED_INTO_BUW_NAMESPACE(Image3f)
BLUE_BLUEFRAMEWORK_IMAGEPROCESSING_EMBED_INTO_BUW_NAMESPACE(Image3d)
BLUE_BLUEFRAMEWORK_IMAGEPROCESSING_EMBED_INTO_BUW_NAMESPACE(Image4b)
BLUE_BLUEFRAMEWORK_IMAGEPROCESSING_EMBED_INTO_BUW_NAMESPACE(Image4f)
BLUE_BLUEFRAMEWORK_IMAGEPROCESSING_EMBED_INTO_BUW_NAMESPACE(Image4d)

#endif // end define BlueFramework_ImageProcessing_Color_f5371cca_cd2f_43c7_b795_35427ef324dc_h
