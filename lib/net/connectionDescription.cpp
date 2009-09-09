
/* Copyright (c) 2005-2009, Stefan Eilemann <eile@equalizergraphics.com> 
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

#include "connectionDescription.h"

#include <sstream>

using namespace std;

namespace eq
{
namespace net
{

#define SEPARATOR '#'

string ConnectionDescription::toString() const
{
    ostringstream description;
    serialize( description );
    return description.str();
}

void ConnectionDescription::serialize( std::ostream& os ) const
{
    switch( type )
    {
        case CONNECTIONTYPE_TCPIP:
            os << "TCPIP";
            break;

        case CONNECTIONTYPE_SDP:
            os << "SDP";
            break;

        case CONNECTIONTYPE_PIPE:
            os << "ANON_PIPE";
            break;

        case CONNECTIONTYPE_NAMEDPIPE:
            os << "PIPE";
            break;

        case CONNECTIONTYPE_IB:
            os << "IB";
            break;
    }        

    os << SEPARATOR << bandwidth << SEPARATOR << _launchCommand 
       << SEPARATOR << static_cast<int>( launchCommandQuote )
       << SEPARATOR << launchTimeout << SEPARATOR << _hostname ;
    
    switch( type )
    {
        case CONNECTIONTYPE_TCPIP:
        case CONNECTIONTYPE_SDP:
        case CONNECTIONTYPE_IB:        
            os << SEPARATOR << TCPIP.port;
            break;
        case CONNECTIONTYPE_NAMEDPIPE:
            os << SEPARATOR << _filename;
            break;

        default:
            break;
    }
    os << SEPARATOR;
}

bool ConnectionDescription::fromString( std::string& data )
{
    {
        size_t nextPos = data.find( SEPARATOR );
        // assume hostname[:port[:type]|:type] or filename:PIPE format
        if( nextPos == string::npos )
        {
            type     = CONNECTIONTYPE_TCPIP;
            nextPos = data.find( ':' );
            if( nextPos == string::npos ) // assume hostname format
            {
                _hostname = data;
                data.clear();
                return true;
            }

            _hostname = data.substr( 0, nextPos );
            data      = data.substr( nextPos + 1 );

            while( nextPos != string::npos )
            {
                nextPos            = data.find( ':' );
                const string token = data.substr( 0, nextPos );
                data               = data.substr( nextPos + 1 );
                
                if( !token.empty() && isdigit( token[0] )) // port
                    TCPIP.port = atoi( token.c_str( ));
                else if( token == "TCPIP" )
                    type = CONNECTIONTYPE_TCPIP;
                else if( token == "SDP" )
                    type = CONNECTIONTYPE_SDP;
                else if( token == "IB" )
                    type = CONNECTIONTYPE_IB;
                else if( token == "PIPE" )
                {
                    type = CONNECTIONTYPE_NAMEDPIPE;
                    setFilename( _hostname );
                    _hostname ="";
                }
                else
                    goto error;
            }

            data.clear();
            return true;
        }

        // else assume SEPARATOR-delimeted list
        const string typeStr = data.substr( 0, nextPos );
        data                 = data.substr( nextPos + 1 );

        if( typeStr == "TCPIP" )
            type = CONNECTIONTYPE_TCPIP;
        else if( typeStr == "SDP" )
            type = CONNECTIONTYPE_SDP;
        else if( typeStr == "ANON_PIPE" )
            type = CONNECTIONTYPE_PIPE;
        else if( typeStr == "PIPE" )
            type = CONNECTIONTYPE_NAMEDPIPE;
        else if( typeStr == "IB" )
            type = CONNECTIONTYPE_IB;
        else
            goto error;

        nextPos = data.find( SEPARATOR );
        if( nextPos == string::npos )
            goto error;

        const string bandwidthStr = data.substr( 0, nextPos );
        data                      = data.substr( nextPos + 1 );
        bandwidth = atoi( bandwidthStr.c_str( ));
    
        nextPos = data.find( SEPARATOR );
        if( nextPos == string::npos )
            goto error;
        _launchCommand = data.substr( 0, nextPos );
        data           = data.substr( nextPos + 1 );

        nextPos = data.find( SEPARATOR );
        if( nextPos == string::npos )
            goto error;
        const string quoteStr = data.substr( 0, nextPos );
        launchCommandQuote = static_cast< char >( atoi( quoteStr.c_str( )));
        data               = data.substr( nextPos + 1 );

        nextPos = data.find( SEPARATOR );
        if( nextPos == string::npos )
            goto error;

        const string launchTimeoutStr = data.substr( 0, nextPos );
        data                          = data.substr( nextPos + 1 );
        launchTimeout = atoi( launchTimeoutStr.c_str( ));

        nextPos = data.find( SEPARATOR );
        if( nextPos == string::npos )
            goto error;

        _hostname = data.substr( 0, nextPos );
        data      = data.substr( nextPos + 1 );

        switch( this->type )
        {
            case CONNECTIONTYPE_TCPIP:
            case CONNECTIONTYPE_SDP:
            case CONNECTIONTYPE_IB:
            {
                nextPos = data.find( SEPARATOR );
                if( nextPos == string::npos )
                    goto error;
            
                const string port = data.substr( 0, nextPos );
                data              = data.substr( nextPos + 1 );
                TCPIP.port        = atoi( port.c_str( ));
                break;
            }
            case CONNECTIONTYPE_NAMEDPIPE:
            {
                nextPos = data.find( SEPARATOR );
                if( nextPos == string::npos )
                    goto error;
                
                _filename         = data.substr( 0, nextPos );
                data              = data.substr( nextPos + 1 );
                break;
            }
            default:
                break;
        }
    }
    return true;

  error:
    EQWARN << "Could not parse connection description: " << data << endl;
    return false;
}

void ConnectionDescription::setHostname( const std::string& hostname )
{
    _hostname = hostname;
}

void ConnectionDescription::setFilename( const std::string& filename )
{
    _filename = filename;
}

const std::string& ConnectionDescription::getFilename() const
{
    return _filename;
}
const string& ConnectionDescription::getHostname() const
{
    return _hostname;
}

void ConnectionDescription::setLaunchCommand( const std::string& launchCommand )
{
    _launchCommand = launchCommand;
}

const string& ConnectionDescription::getLaunchCommand() const
{
    return _launchCommand;
}

EQ_EXPORT std::ostream& operator << ( std::ostream& os, 
                                      const ConnectionDescription* desc)
{
    if( !desc )
    {
        os << "NULL connection description";
        return os;
    }

    os << "connection " << ( desc->type == CONNECTIONTYPE_TCPIP ? "tcp/ip":
                             desc->type == CONNECTIONTYPE_SDP   ? "sdp"   :
                             desc->type == CONNECTIONTYPE_PIPE  ? "anonpipe" :
                             desc->type == CONNECTIONTYPE_NAMEDPIPE  ? "pipe" :
                             desc->type == CONNECTIONTYPE_IB  ? "ib"  :
                             "ERROR" ) 
       << ' ' << desc->getHostname() << ':';

    switch( desc->type )
    {
        case CONNECTIONTYPE_TCPIP:
        case CONNECTIONTYPE_SDP:
        case CONNECTIONTYPE_IB:
            os << desc->TCPIP.port;
            break;

        case CONNECTIONTYPE_NAMEDPIPE:
            os << desc->getFilename();
            break;

        default:
        case CONNECTIONTYPE_PIPE:
            break;
    }

    return os;
}


}
}
