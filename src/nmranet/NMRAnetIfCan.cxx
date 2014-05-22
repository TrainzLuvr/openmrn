/** \copyright
 * Copyright (c) 2013, Stuart W Baker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are  permitted provided that the following conditions are met:
 * 
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \file NMRAnetIfCan.cxx
 * This file provides an NMRAnet interface specific to CAN.
 *
 * @author Stuart W. Baker
 * @date 18 September 2013
 */

#include "nmranet/NMRAnetIfCan.hxx"

#if defined (__linux__) || defined (__MACH__)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#elif defined (__FreeRTOS__)
#include <stropts.h>
#include <can_ioctl.h>
#endif
#include <endian.h>
#include <unistd.h>

#include "nmranet/NMRAnetDatagram.hxx"
#include "nmranet/NMRAnetNode.hxx"

namespace nmranet
{



/** Get the NMRAnet MTI from a can identifier.
 * @param can_id CAN identifider
 * @return NMRAnet MTI
 */
Defs::MTI CanDefs::nmranet_mti(uint32_t can_id)
{
    switch (get_can_frame_type(can_id))
    {
        default:
            return MTI_NONE;
        case GLOBAL_ADDRESSED:
            return (MTI)get_mti(can_id);
        case DATAGRAM_ONE_FRAME:
            /* fall through */
        case DATAGRAM_FIRST_FRAME:
            /* fall through */
        case DATAGRAM_MIDDLE_FRAME:
            /* fall through */
        case DATAGRAM_FINAL_FRAME:
            return MTI_DATAGRAM;
        case STREAM_DATA:
            return MTI_STREAM_DATA;
    }
}

/** Get the CAN identifier from an NMRAnet mti and source alias.
 * @param mti NMRAnet MTI
 * @param src Source node alias
 * @return CAN identifier
 */
uint32_t CanDefs::can_identifier(MTI mti, NodeAlias src)
{
    return ((src << SRC_SHIFT           ) & SRC_MASK) +
           ((mti << MTI_SHIFT           ) & MTI_MASK) +
           ((1   << CAN_FRAME_TYPE_SHIFT)           ) +
           ((1   << FRAME_TYPE_SHIFT    )           ) +
           ((1   << PRIORITY_SHIFT      )           );
}

}; /* namespace nmranet */
