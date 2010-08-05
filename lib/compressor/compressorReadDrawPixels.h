
/* Copyright (c) 2010, Cedric Stalder <cedric.stalder@gmail.com>
 *               2010, Stefan Eilemann <eile@eyescale.ch>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
 
#ifndef EQ_PLUGIN_COMPRESSORREADDRAWPIXELS
#define EQ_PLUGIN_COMPRESSORREADDRAWPIXELS

#include "compressor.h"

#include <GL/glew.h>

namespace eq
{
namespace util{ class Texture; }

namespace plugin
{

class CompressorReadDrawPixels : public Compressor
{
public:
    CompressorReadDrawPixels( const EqCompressorInfo* info );
    virtual ~CompressorReadDrawPixels();
    
    static void* getNewCompressor( const EqCompressorInfo* info )
        { return new CompressorReadDrawPixels( info ); }

    static void* getNewDecompressor( const EqCompressorInfo* info )
        { return new CompressorReadDrawPixels( info ); }

    virtual void compress( const void* const inData, 
                           const eq_uint64_t nPixels, 
                           const bool        useAlpha )
        { EQDONTCALL; }
    
    static bool isCompatible( const GLEWContext* glewContext );
    
    void download( const GLEWContext* glewContext,
                   const uint64_t     inDims[4],
                   const unsigned     source,
                   const uint64_t     flags,
                   uint64_t           outDims[4],
                   void**             out );

    void upload( const GLEWContext* glewContext, 
                 const void*        buffer,
                 const uint64_t     inDims[4],
                 const uint64_t     flags,
                 const uint64_t     outDims[4],  
                 const unsigned     destination );

protected:
    eq::base::Bufferb _buffer;
    util::Texture*    _texture;
    uint32_t    _internalFormat; //!< the GL format
    uint32_t    _format;         //!< the GL format
    uint32_t    _type;           //!< the GL type 
    uint32_t    _depth;          //!< the size of one output token

    void _init( const uint64_t  inDims[4], uint64_t  outDims[4] );
};

}
}
#endif // EQ_PLUGIN_COMPRESSORREADDRAWPIXELS