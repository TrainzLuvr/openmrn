/** \copyright
 * Copyright (c) 2014, Balazs Racz
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
 * \file RailcomBroadcastDecoder.hxx
 *
 * Simple state machine to decode DCC address from railcom broadcast packets.
 *
 * @author Balazs Racz
 * @date 11 Jan 2015
 */

#include <stdint.h>

#ifndef _DCC_RAILCOMBROADCASTDECODER_HXX_
#define _DCC_RAILCOMBROADCASTDECODER_HXX_

namespace dcc
{

class Feedback;

class RailcomBroadcastDecoder
{
public:
    RailcomBroadcastDecoder()
        : currentH_(0)
        , currentL_(0)
        , countH_(0)
        , countL_(0)
        , currentAddress_(0)
        , lastAddress_(0)
    {
    }

    /** Returns the currently valid DCC address, or zero if no valid address
     * right now. */
    uint16_t current_address()
    {
        return currentAddress_;
    }

    /** Decodes a packet.
     *
     * @returns true if the packet is garbage or matches a global address
     * broadcast, false if it is a valid packet that is not an address
     * broadcast. */
    bool process_packet(const dcc::Feedback &packet);

    /** Notifies the state machine about observed occupancy.
     *
     * @param value is true if the track is sensed as occupied. */
    void set_occupancy(bool value);

private:
    bool process_data(const uint8_t *data, unsigned size);

    static const uint8_t REPEAT_COUNT = 3;

    uint8_t currentH_; //< last received high address bits
    uint8_t currentL_; //< last received low address bits
    uint8_t countH_;   //< observed repeat count of high address bits
    uint8_t countL_;   //< observed repeat count of low address bits

    uint16_t currentAddress_; //< last valid address (0 if no valid address)

public:
    uint16_t lastAddress_; //< usable by clients for storage.
};

} // namespace dcc

#endif // _DCC_RAILCOMBROADCASTDECODER_HXX_