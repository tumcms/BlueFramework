/*! \verbatim
 *  \copyright      Copyright (c) 2016 Julian Amann. All rights reserved.
 *  \author         Julian Amann <dev@vertexwahn.de> (http://vertexwahn.de/)
 *  \brief          This file is part of the BlueFramework.
 *  \endverbatim
 */
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

#pragma once

#include <unordered_map>
#include <memory>

#include "BlueFramework/Rasterizer/IRenderSystem.h"
#include "BlueFramework/Core/class.h"
#include "Rect.h"

namespace BlueFramework { namespace Engine {

class SignedDistanceFont
{
public:

	// stores the font metrics for a single character
	struct Metrics 
	{
		float x1;	// x - top left of character in texture
		float y1;	// y - top left of character in texture
		float x2;	// x - bottom right of character in texture
		float y2;	// y - bottom right of character in texture
		float w;	// w - width of character
		float h;	// h - height of character
		float dx;	// xoffset - adjusts character positioning
		float dy;	// yoffset - adjusts character positioning
		float d;	// xadvance - adjusts character positioning
	};

	typedef std::unordered_map<uint16_t, Metrics>	MetricsData;
	

public:
	SignedDistanceFont(void);
	~SignedDistanceFont(void);

	//! creates a font from the two files generated by LoneSock's SDFont.exe
	void create( const char* fontFileName, const char* textureFilename, buw::ReferenceCounted<buw::IRenderSystem> renderSystem );
	//! reads a binary font file created using 'writeBinary'
	//void read( const ci::DataSourceRef source );
	//! writes the font to a binary file
	//void write( const ci::DataTargetRef target );

	//!
	std::string getFamily() const { return mFamily; }

	//!
	float		getAscent( float fontSize=12.0f ) const { return mAscent * (fontSize / mFontSize); }
	//!
	float		getDescent( float fontSize=12.0f ) const { return mDescent * (fontSize / mFontSize); }
	//!
	float		getLeading( float fontSize=12.0f ) const { return mLeading * (fontSize / mFontSize); }
	//!
	float		getSpaceWidth( float fontSize=12.0f ) const { return mSpaceWidth * (fontSize / mFontSize); }

	//!
	bool		contains(uint16_t charcode) const { return ( mMetrics.find(charcode) != mMetrics.end() ); }

	//!
	Metrics		getMetrics(uint16_t charcode) const;

	//!
	Rectf	getBounds(uint16_t charcode, float fontSize=12.0f) const;
	//!
	inline Rectf getBounds(const Metrics &metrics, float fontSize=12.0f) const;
	//!
	Rectf	getTexCoords(uint16_t charcode) const;
	//!
	inline Rectf getTexCoords(const Metrics &metrics) const;
	//!
	float		getAdvance(uint16_t charcode, float fontSize=12.0f) const;
	//!
	inline float getAdvance(const Metrics &metrics, float fontSize=12.0f) const;

	//!
	//void		enableAndBind() const { if(mTexture) mTexture.enableAndBind(); }
	//!
	//void		bind(GLuint textureUnit=0) const { if(mTexture) mTexture.bind(textureUnit); }
	//!
	//void		unbind(GLuint textureUnit=0) const  { if(mTexture) mTexture.unbind(textureUnit); }

	buw::ReferenceCounted<buw::ITexture2D> getTexture() { return mTexture; }

	//!
	//Rectf	measure( const std::string& text, float fontSize=12.0f ) const { return measure( ci::toUtf16(text), fontSize ); }
	//!
	Rectf	measure( const std::wstring &text, float fontSize=12.0f ) const;

	//!
	//float		measureWidth( const std::string& text, float fontSize=12.0f, bool precise=true ) const { return measureWidth( ci::toUtf16(text), fontSize, precise ); }
	//!
	float		measureWidth( const std::wstring &text, float fontSize=12.0f, bool precise=true ) const;

protected:
	bool				mInvalid;

	std::string			mFamily;

	//! calculated by the 'measure' function
	float				mFontSize;
	float				mLeading;
	float				mAscent;
	float				mDescent;
	float				mSpaceWidth;

	//ci::Surface			mSurface;
	//ci::gl::Texture		mTexture;
	buw::ReferenceCounted<buw::ITexture2D>   mTexture;
	buw::Vector2f			mTextureSize;

	MetricsData			mMetrics;
};

class FontExc : public std::exception 
{
public:
	virtual const char* what() const throw() { return "Font exception"; }
};

class FontInvalidSourceExc : public FontExc 
{
public:
	virtual const char* what() const throw() { return "Font exception: could not load from the specified source"; }
};

class FontInvalidTargetExc : public FontExc 
{
public:
	virtual const char* what() const throw() { return "Font exception: could not write to the specified target"; }
};

} } 

namespace buw
{
	using BlueFramework::Engine::SignedDistanceFont;
}

