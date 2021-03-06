
/* Copyright (c) 2013, Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#ifndef EQ_GLX_SAGE_EVENTHANDLER_H
#define EQ_GLX_SAGE_EVENTHANDLER_H

#include <eq/client/eventHandler.h> // base class
#include <eq/client/types.h>        // basic typedefs

#include <lunchbox/thread.h> // thread-safety macro

namespace eq
{
namespace glx
{
    /** @internal The event handler for one SAGE view. */
    class SageEventHandler : public eq::EventHandler
    {
    public:
        /** Construct a new SAGE event handler. @version 1.5.2 */
        SageEventHandler( SageProxy* sage );

        /** Destruct the SAGE event handler. @version 1.5.2 */
        virtual ~SageEventHandler();

        /**
         * Dispatch all pending events on the current thread.
         *
         * If no event handlers have been constructed by the calling thread,
         * this function does nothing. This function does not block on events.
         *
         * @param sage if not 0, limit processing to the given sage instance.
         * @version 1.5.2
         */
        static void processEvents( const SageProxy* sage = 0 );

    private:
        /** The corresponding SAGE proxy instance. */
        SageProxy* const _sage;

        void _processEvents( const SageProxy* sage );

        LB_TS_VAR( _thread );
    };
}
}
#endif // EQ_GLX_SAGE_EVENTHANDLER_H

