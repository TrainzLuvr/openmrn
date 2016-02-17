/** \copyright
 * Copyright (c) 2013, Balazs Racz
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
 * \file Hub.hxx
 * Implementation of the pipe dispatcher flow.
 *
 * @author Balazs Racz
 * @date 8 Dec 2013
 */

#ifndef _UTILS_HUB_HXX_
#define _UTILS_HUB_HXX_

#include <stdint.h>
#include <string>

#include "executor/Dispatcher.hxx"
#include "can_frame.h"

class PipeBuffer;
class PipeMember;

/// Container for an arbitrary structure to pass through a Hub.
template<class S> class StructContainer : public S {
public:
    S& value() {
        return *this;
    }

    const void* data() const {
        return &value();
    }

    void* data() {
        return &value();
    }

    size_t size() {
        return sizeof(S);
    }
};

/// Container for (binary) CAN frames going through Hubs.
struct CanFrameContainer : public StructContainer<can_frame>
{
    /* Constructor. Sets up (outgoing) frames to be empty extended frames by
     * default. */
    CanFrameContainer()
    {
        CLR_CAN_FRAME_ERR(*this);
        CLR_CAN_FRAME_RTR(*this);
        SET_CAN_FRAME_EFF(*this);
        can_dlc = 0;
    }

    /** @returns a mutable pointer to the embedded CAN frame. */
    struct can_frame *mutable_frame()
    {
        return this;
    }
    /** @returns the embedded CAN frame. */
    const struct can_frame &frame() const
    {
        return *this;
    }
};

/// Data type wrapper for sending data through a Hub. It adds the @ref
/// skipMember_ to the structure that represents for a HubFlow to decide where
/// the data is coming from. This is needed to avoid the Hub performing
/// loopback.
template <class T> class HubContainer : public T
{
public:
    // typedef FlowInterface<Buffer<HubContainer<T>>> HubMember;
    HubContainer() : skipMember_(0)
    {
    }
    typedef uintptr_t id_type;
    /// Defines which registered member of the hub should be skipped when the
    /// output members are enumerated.
    FlowInterface<Buffer<HubContainer<T>>> *skipMember_;
    /// Defines the indentifier used for the DispatchFlow inside the Hub.
    id_type id()
    {
        return reinterpret_cast<uintptr_t>(skipMember_);
    }
};

/** This class can be sent via a Buffer to a hub.
 *
 * Access the data content via members char* data() and size_t size().
 *
 * Set skipMember_ to non-NULL to skip a particular entry flow of the output.
 */
typedef HubContainer<string> HubData;

/** This class can be sent via a Buffer to a CAN hub.
 *
 * Access the data content via members \ref mutable_frame and \ref frame.
 *
 * Set skipMember_ to non-NULL to skip a particular entry flow of the output.
 */
typedef HubContainer<CanFrameContainer> CanHubData;

/** All ports interfacing via a hub will have to derive from this flow. */
typedef FlowInterface<Buffer<HubData>> HubPortInterface;
typedef StateFlow<Buffer<HubData>, QList<1>> HubPort;
typedef FlowInterface<Buffer<CanHubData>> CanHubPortInterface;
typedef StateFlow<Buffer<CanHubData>, QList<1>> CanHubPort;

/// This should work for both 32 and 64-bit architectures.
static const uintptr_t POINTER_MASK = UINTPTR_MAX;

/// Templated implementation of the HubFlow.
template<class D> class GenericHubFlow : public DispatchFlow<Buffer<D>, 1>
{
public:
    typedef D value_type;
    typedef Buffer<value_type> buffer_type;
    typedef FlowInterface<buffer_type> port_type;

    GenericHubFlow(Service *s) : DispatchFlow<Buffer<D>, 1>(s)
    {
        this->negateMatch_ = true;
    }

    void register_port(port_type *port)
    {
        this->register_handler(port, reinterpret_cast<uintptr_t>(port),
                               POINTER_MASK);
    }

    void unregister_port(port_type *port)
    {
        this->unregister_handler(port, reinterpret_cast<uintptr_t>(port),
                                 POINTER_MASK);
    }
};

/** A generic hub that proxies packets of untyped (aka string) data. */
typedef GenericHubFlow<HubData> HubFlow;
/** A hub that proxies packets of CAN frames. */
typedef GenericHubFlow<CanHubData> CanHubFlow;

/** This port prints all traffic from a (string-typed) hub to stdout. */
class DisplayPort : public HubPort
{
public:
    DisplayPort(Service *service, bool timestamped)
        : HubPort(service)
        , timestamped_(timestamped)
    {
    }

    Action entry() override
    {
        string s(message()->data()->data(), message()->data()->size());
        if (timestamped_) {
            long long ts = os_get_time_monotonic();
            printf("%lld.%06lld: %s", ts / 1000000000, (ts / 1000) % 1000000,
                   s.c_str());
        } else {
            printf("%s", s.c_str());
        }
        return release_and_exit();
    }

private:
    bool timestamped_;
};

#endif // _UTILS_HUB_HXX_
