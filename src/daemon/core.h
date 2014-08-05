// Copyright (c) 2014, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "cryptonote_core/checkpoints_create.h"
#include "cryptonote_core/cryptonote_core.h"
#include "cryptonote_protocol/cryptonote_protocol_handler.h"
#include "misc_log_ex.h"
#include <stdexcept>
#include <boost/program_options.hpp>

namespace daemonize
{

class t_core final
{
public:
  static void init_options(boost::program_options::options_description & option_spec)
  {
    cryptonote::core::init_options(option_spec);
    cryptonote::miner::init_options(option_spec);
  }
private:
  typedef cryptonote::t_cryptonote_protocol_handler<cryptonote::core> t_protocol_raw;
  cryptonote::core m_core;
public:
  t_core(
      boost::program_options::variables_map const & vm
    )
    : m_core{nullptr}
  {
    cryptonote::checkpoints checkpoints;
    if (!cryptonote::create_checkpoints(checkpoints))
    {
      throw std::runtime_error("Failed to initialize checkpoints");
    }
    m_core.set_checkpoints(std::move(checkpoints));

    //initialize core here
    LOG_PRINT_L0("Initializing core...");
    if (!m_core.init(vm))
    {
      throw std::runtime_error("Failed to initialize core");
    }
    LOG_PRINT_L0("Core initialized OK");
  }

  // TODO - get rid of circular dependencies in internals
  void set_protocol(t_protocol_raw & protocol)
  {
    m_core.set_cryptonote_protocol(&protocol);
  }

  cryptonote::core & get()
  {
    return m_core;
  }

  ~t_core()
  {
    LOG_PRINT_L0("Deinitializing core...");
    try {
      m_core.deinit();
      m_core.set_cryptonote_protocol(nullptr);
    } catch (...) {
      LOG_PRINT_L0("Failed to deinitialize core...");
    }
  }
};

}