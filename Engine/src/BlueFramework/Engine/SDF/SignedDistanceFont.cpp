/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "SignedDistanceFont.h"
#include <boost/algorithm/string.hpp>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "BlueFramework/Core/Exception.h"
#include "BlueFramework/ImageProcessing/Image.h"

#undef min
#undef max

#include <fstream>

std::string loadString( const char *filename )
{
	if (!boost::filesystem::exists(filename))
	{
		BLUE_LOG_STREAM_EX(
			"BlueFramework::Rasterizer::OpenGL4x::ShaderBase::getFileContentAsString",
			buw::eLogSeverityLevel::Error)
			<< "File" << filename << " does not exist";

		throw buw::FileNotFoundException(filename);
	}

	std::stringstream ss;
	std::ifstream inFile;
	inFile.open(filename);

	if (inFile.is_open())
	{
		while ( inFile.good() )
		{
			std::string line;
			std::getline (inFile,line);
			ss << line << std::endl;
		}
	}

	return ss.str();
}

std::vector<std::string> split( const std::string& str, const std::string& separators, bool compress = true )
{
	std::vector<std::string> result;

	boost::algorithm::split( result, str, boost::is_any_of(separators),
		compress ? boost::token_compress_on : boost::token_compress_off );

	return result;
}

namespace BlueFramework { namespace Engine {

using namespace std;



SignedDistanceFont::SignedDistanceFont(void)
	:
	mInvalid(true),
	mFamily("Unknown"),
	mFontSize(12.0f),
	mLeading(0.0f),
	mAscent(0.0f),
	mDescent(0.0f),
	mSpaceWidth(0.0f),
	mTexture(nullptr)
{
}

SignedDistanceFont::~SignedDistanceFont(void)
{
}

void SignedDistanceFont::create( const char* fontFileName, const char* textureFilename, buw::ReferenceCounted<buw::IRenderSystem> renderSystem )
{
	// initialize
	mInvalid = true;
	mFamily = "Unknown";
	mFontSize = 12.0f;
	mLeading = 0.0f;
	mAscent = 0.0f;
	mDescent = 0.0f;
	mSpaceWidth = 0.0f;

	mMetrics.clear();

	// try to load the font texture
	try {

		buw::ReferenceCounted<buw::Image_4b> image = buw::loadImageFromFile<buw::Image_4b>(textureFilename);

		buw::texture2DDescription t2d;
		t2d.format = buw::eTextureFormat::R8G8B8A8_UnsignedNormalizedInt;
		t2d.width = image->getWidth();
		t2d.height = image->getHeight();
		t2d.data = image->getData();
		t2d.mipMapCount = t2d.getMaxNumMipLevels();
	 	mTexture = renderSystem->createTexture2D(t2d);
		mTextureSize = buw::vector2f(image->getWidth(), image->getHeight());
// 		mSurface = ci::Surface( loadImage( png ) );
// 		mTexture = gl::Texture( mSurface );
//		mTextureSize = mTexture.getSize();
	}
	catch( ... ) { throw FontInvalidSourceExc();	}

	// now that we have the font, let's read the metrics file
	std::string	data;
	try
	{
		data = loadString( fontFileName );
	}
	catch( ... ) { throw FontInvalidSourceExc();	}

	// parse the file
	try {
		// split the file into lines
		std::vector<std::string> lines = split(data, "\n\r");
		if(lines.size() < 2) throw;

		// read the first line, containing the font name
		std::vector<std::string> tokens = split(lines[0], "=");
		if(tokens.size() < 2 && tokens[0] != "info face") throw;

		mFamily = boost::algorithm::erase_all_copy( tokens[1], "\"" );

		// read the second containing the number of characters in this font
		tokens = split(lines[1], "=");
		if(tokens.size() < 2 && tokens[0] != "chars count") throw;

		int count = boost::lexical_cast<int>( tokens[1] );
		if(count < 1) throw;

		// create the metrics
		uint32_t charcode = 0;
		for(int i=2;i<count+2;++i) {
			tokens = split(lines[i], " ");
			Metrics m;
			for(size_t j=0;j<tokens.size();++j) {
				std::vector<std::string> kvp = split(tokens[j], "=");
				if(kvp.size() < 2) continue;

				if(kvp[0] == "id")
					charcode = boost::lexical_cast<uint32_t>( kvp[1] );
				else if(kvp[0] == "x")
					m.x1 = boost::lexical_cast<float>( kvp[1] );
				else if(kvp[0] == "y")
					m.y1 = boost::lexical_cast<float>( kvp[1] );
				else if(kvp[0] == "width")
					m.w = boost::lexical_cast<float>( kvp[1] );
				else if(kvp[0] == "height")
					m.h = boost::lexical_cast<float>( kvp[1] );
				else if(kvp[0] == "xoffset")
					m.dx = boost::lexical_cast<float>( kvp[1] );
				else if(kvp[0] == "yoffset")
					m.dy = boost::lexical_cast<float>( kvp[1] );
				else if(kvp[0] == "xadvance")
					m.d = boost::lexical_cast<float>( kvp[1] );
			}

			m.x2 = m.x1 + m.w;
			m.y2 = m.y1 + m.h;
			mMetrics[charcode] = m;
		}
	}
	catch( ... ) { throw FontInvalidSourceExc(); }

	// measure font (standard ASCII range only to prevent weird characters influencing the measurements)
	for(uint16_t i=33;i<127;++i) {
		MetricsData::const_iterator itr = mMetrics.find(i);
		if(itr != mMetrics.end()) {
			mAscent = std::max( mAscent, itr->second.dy );
			mDescent = std::max( mDescent, itr->second.h - itr->second.dy );
		}
	}

	mLeading = mAscent + mDescent;
	mFontSize = mAscent + mDescent;

	if( mMetrics.find(32) != mMetrics.end() )
		mSpaceWidth = mMetrics[32].d;
}
/*
void Font::read(const ci::DataSourceRef source)
{
	mInvalid = true;

	IStreamRef	in = source->createStream();
	size_t		filesize = in->size();

	// read header
	uint8_t header;
	in->read( &header ); if(header != 'S') throw FontInvalidSourceExc();
	in->read( &header ); if(header != 'D') throw FontInvalidSourceExc();
	in->read( &header ); if(header != 'F') throw FontInvalidSourceExc();
	in->read( &header ); if(header != 'F') throw FontInvalidSourceExc();

	uint16_t version;
	in->readLittle( &version );

	// read font name
	if( version > 0x0001 ) in->read( &mFamily );

	// read font data
	in->readData( (void*) &mLeading, sizeof(mLeading) );
	in->readData( (void*) &mAscent, sizeof(mAscent) );
	in->readData( (void*) &mDescent, sizeof(mDescent) );
	in->readData( (void*) &mSpaceWidth, sizeof(mSpaceWidth) );
	mFontSize = mAscent + mDescent;

	// read metrics data
	mMetrics.clear();

	try {
		uint16_t count;
		in->readLittle( &count );

		for(int i=0;i<count;++i) {
			uint16_t charcode;
			in->readLittle( &charcode );

			Metrics m;
			in->readData( (void*) &(m.x1), sizeof(m.x1) );
			in->readData( (void*) &(m.y1), sizeof(m.y1) );
			in->readData( (void*) &(m.w), sizeof(m.w) );
			in->readData( (void*) &(m.h), sizeof(m.h) );

			in->readData( (void*) &(m.dx), sizeof(m.dx) );
			in->readData( (void*) &(m.dy), sizeof(m.dy) );
			in->readData( (void*) &(m.d), sizeof(m.d) );

			m.x2 = m.x1 + m.w;
			m.y2 = m.y1 + m.h;
			mMetrics[charcode] = m;
		}
	}
	catch( ... ) {
		throw FontInvalidSourceExc();
	}

	// read image data
	try {
		// reserve memory
		Buffer buffer(filesize);
		// read the remaining data into memory (needs fix from Github's Cinder, won't work with release version)
		size_t bytesRead = in->readDataAvailable(buffer.getData(), filesize);

		// load image
		mSurface = Surface( loadImage( DataSourceBuffer::create(buffer), ImageSource::Options(), "png" ) );

		// apply mip-mapping
		gl::Texture::Format fmt;
		fmt.enableMipmapping();
		fmt.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
		fmt.setMagFilter( GL_LINEAR );

		mTexture = gl::Texture( mSurface, fmt );
		mTextureSize = mTexture.getSize();
	}
	catch( ... ) {
		throw FontInvalidSourceExc();
	}
}

void Font::write(const ci::DataTargetRef target)
{
	if(!target) throw FontInvalidTargetExc();

	OStreamRef out = target->getStream();

	// write header
	out->write( (uint8_t) 'S' );
	out->write( (uint8_t) 'D' );
	out->write( (uint8_t) 'F' );
	out->write( (uint8_t) 'F' );

	uint16_t version = 0x0002;
	out->writeLittle( version );

	// write font name
	out->write(mFamily);

	// write font data
	out->writeData( (void*) &mLeading, sizeof(mLeading) );
	out->writeData( (void*) &mAscent, sizeof(mAscent) );
	out->writeData( (void*) &mDescent, sizeof(mDescent) );
	out->writeData( (void*) &mSpaceWidth, sizeof(mSpaceWidth) );

	// write metrics data
	{
		uint16_t count = (uint16_t) mMetrics.size();
		out->writeLittle( count );

		MetricsData::const_iterator itr;
		for(itr=mMetrics.begin();itr!=mMetrics.end();++itr) {
			// write char code
			out->writeLittle( itr->first );
			// write metrics
			out->writeData( (void*) &(itr->second.x1), sizeof(itr->second.x1) );
			out->writeData( (void*) &(itr->second.y1), sizeof(itr->second.y1) );
			out->writeData( (void*) &(itr->second.w), sizeof(itr->second.w) );
			out->writeData( (void*) &(itr->second.h), sizeof(itr->second.h) );

			out->writeData( (void*) &(itr->second.dx), sizeof(itr->second.dx) );
			out->writeData( (void*) &(itr->second.dy), sizeof(itr->second.dy) );
			out->writeData( (void*) &(itr->second.d), sizeof(itr->second.d) );
		}
	}

	// write image data
	writeImage( DataTargetStream::createRef(out), mSurface.getChannelRed(), ImageTarget::Options(), "png" );
}
*/
SignedDistanceFont::Metrics SignedDistanceFont::getMetrics(uint16_t charcode) const
{
	MetricsData::const_iterator itr = mMetrics.find(charcode);
	if( itr == mMetrics.end() ) return Metrics();

	return itr->second;
}

Rectf SignedDistanceFont::getBounds(uint16_t charcode, float fontSize) const
{
	MetricsData::const_iterator itr = mMetrics.find(charcode);
	if( itr != mMetrics.end() )
		return getBounds( itr->second, fontSize );
	else
		return Rectf();
}

Rectf SignedDistanceFont::getBounds(const Metrics &metrics, float fontSize) const
{
	float	scale = (fontSize / mFontSize);

	return Rectf(
		buw::vector2f(metrics.dx, -metrics.dy) * scale,
		buw::vector2f(metrics.dx + metrics.w, metrics.h - metrics.dy) * scale
	);
}

Rectf SignedDistanceFont::getTexCoords(uint16_t charcode) const
{
	MetricsData::const_iterator itr = mMetrics.find(charcode);
	if( itr != mMetrics.end() )
		return getTexCoords( itr->second );
	else
		return Rectf();
}

Rectf SignedDistanceFont::getTexCoords(const Metrics &metrics) const
{
	return Rectf(
		buw::vector2f(metrics.x1 / mTextureSize.x(), metrics.y1 / mTextureSize.y()),
		buw::vector2f(metrics.x2 / mTextureSize.x(), metrics.y2 /  mTextureSize.y())
	);
}

float SignedDistanceFont::getAdvance(uint16_t charcode, float fontSize) const
{
	MetricsData::const_iterator itr = mMetrics.find(charcode);
	if( itr != mMetrics.end() )
		return getAdvance( itr->second, fontSize );

	return 0.0f;
}

float SignedDistanceFont::getAdvance(const Metrics &metrics, float fontSize) const
{
	return metrics.d * fontSize / mFontSize;
}

Rectf SignedDistanceFont::measure(const std::wstring &text, float fontSize) const
{
	float offset = 0.0f;
	Rectf result(0.0f, 0.0f, 0.0f, 0.0f);

	std::wstring::const_iterator citr;
	for(citr=text.begin();citr!=text.end();++citr) {
		uint16_t charcode = (uint16_t) *citr;

		// TODO: handle special chars like /t

		MetricsData::const_iterator itr = mMetrics.find(charcode);
		if(itr != mMetrics.end()) {
			result.include(
				Rectf(offset + itr->second.dx, -itr->second.dy,
				offset + itr->second.dx + itr->second.w, itr->second.h - itr->second.dy)
			);
			offset += itr->second.d;
		}
	}

	// return
	return result.scaled( fontSize / mFontSize );
}

float SignedDistanceFont::measureWidth(const std::wstring &text, float fontSize, bool precise) const
{
	float offset = 0.0f;
	float adjust = 0.0f;

	std::wstring::const_iterator citr;
	for(citr=text.begin();citr!=text.end();++citr) {
		uint16_t charcode = (uint16_t) *citr;

		// TODO: handle special chars like /t

		MetricsData::const_iterator itr = mMetrics.find(charcode);
		if(itr != mMetrics.end()) {
			offset += itr->second.d;

			// precise measurement takes into account that the last character
			// contributes to the total width only by its own width, not its advance
			if( precise )
				adjust = itr->second.dx + itr->second.w - itr->second.d;
		}
	}

	return (offset + adjust) * ( fontSize / mFontSize );
}

} } // namespace ph::text
