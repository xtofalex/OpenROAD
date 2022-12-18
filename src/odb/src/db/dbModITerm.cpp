///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2022, The Regents of the University of California
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// Generator Code Begin Cpp
#include "dbModITerm.h"

#include "db.h"
#include "dbBlock.h"
#include "dbDatabase.h"
#include "dbDiff.hpp"
#include "dbModInst.h"
#include "dbTable.h"
#include "dbTable.hpp"
// User Code Begin Includes
#include "dbModNet.h"
// User Code End Includes
namespace odb {

template class dbTable<_dbModITerm>;

bool _dbModITerm::operator==(const _dbModITerm& rhs) const
{
  if (_net != rhs._net)
    return false;

  if (_next_moditerm != rhs._next_moditerm)
    return false;

  if (_prev_moditerm != rhs._prev_moditerm)
    return false;

  if (_next_entry != rhs._next_entry)
    return false;

  if (_inst != rhs._inst)
    return false;

  // User Code Begin ==
  // User Code End ==
  return true;
}
bool _dbModITerm::operator<(const _dbModITerm& rhs) const
{
  // User Code Begin <
  // User Code End <
  return true;
}
void _dbModITerm::differences(dbDiff& diff,
                              const char* field,
                              const _dbModITerm& rhs) const
{
  DIFF_BEGIN

  DIFF_FIELD(_net);
  DIFF_FIELD(_next_moditerm);
  DIFF_FIELD(_prev_moditerm);
  DIFF_FIELD(_next_entry);
  DIFF_FIELD(_inst);
  // User Code Begin Differences
  // User Code End Differences
  DIFF_END
}
void _dbModITerm::out(dbDiff& diff, char side, const char* field) const
{
  DIFF_OUT_BEGIN
  DIFF_OUT_FIELD(_net);
  DIFF_OUT_FIELD(_next_moditerm);
  DIFF_OUT_FIELD(_prev_moditerm);
  DIFF_OUT_FIELD(_next_entry);
  DIFF_OUT_FIELD(_inst);

  // User Code Begin Out
  // User Code End Out
  DIFF_END
}
_dbModITerm::_dbModITerm(_dbDatabase* db)
{
  // User Code Begin Constructor
  // User Code End Constructor
}
_dbModITerm::_dbModITerm(_dbDatabase* db, const _dbModITerm& r)
{
  _net = r._net;
  _next_moditerm = r._next_moditerm;
  _prev_moditerm = r._prev_moditerm;
  _next_entry = r._next_entry;
  _inst = r._inst;
  // User Code Begin CopyConstructor
  // User Code End CopyConstructor
}

dbIStream& operator>>(dbIStream& stream, _dbModITerm& obj)
{
  stream >> obj._net;
  stream >> obj._next_moditerm;
  stream >> obj._prev_moditerm;
  stream >> obj._next_entry;
  stream >> obj._inst;
  // User Code Begin >>
  // User Code End >>
  return stream;
}
dbOStream& operator<<(dbOStream& stream, const _dbModITerm& obj)
{
  stream << obj._net;
  stream << obj._next_moditerm;
  stream << obj._prev_moditerm;
  stream << obj._next_entry;
  stream << obj._inst;
  // User Code Begin <<
  // User Code End <<
  return stream;
}

_dbModITerm::~_dbModITerm()
{
  // User Code Begin Destructor
  // User Code End Destructor
}

// User Code Begin PrivateMethods
// User Code End PrivateMethods

////////////////////////////////////////////////////////////////////
//
// dbModITerm - Methods
//
////////////////////////////////////////////////////////////////////

dbModInst* dbModITerm::getInst() const
{
  _dbModITerm* obj = (_dbModITerm*) this;
  if (obj->_inst == 0)
    return NULL;
  _dbBlock* par = (_dbBlock*) obj->getOwner();
  return (dbModInst*) par->_modinst_tbl->getPtr(obj->_inst);
}

// User Code Begin dbModITermPublicMethods
void dbModITerm::connect(dbModNet* net_)
{
  _dbModITerm* iterm = (_dbModITerm*) this;
  _dbModNet* net = (_dbModNet*) net_;
  _dbBlock* block = (_dbBlock*) iterm->getOwner();

  if (iterm->_net)
    iterm->disconnectNet(iterm, block);
  iterm->connectNet(net, block);
}

void dbModITerm::disconnect()
{
  _dbModITerm* iterm = (_dbModITerm*) this;
  if (iterm->_net) {
    _dbBlock* block = (_dbBlock*) iterm->getOwner();
    // Fixme similar to dbNet
    // see dont_touch management
    // and manage block journal
    //
    iterm->disconnectNet(iterm, block);
  }
}

void _dbModITerm::connectNet(_dbModNet* net, _dbBlock* block)
{
#if 0  // FIXME (xtof) To look at later
  for (auto callback : block->_callbacks)
    callback->inDbBTermPreConnect((dbBTerm*) this, (dbNet*) net);
#endif
  _net = net->getOID();
  if (net->_moditerms != 0) {
    _dbModITerm* tail = block->_moditerm_tbl->getPtr(net->_moditerms);
    _next_moditerm = net->_moditerms;
    tail->_prev_moditerm = getOID();
  } else
    _next_moditerm = 0;
  _prev_moditerm = 0;
  net->_moditerms = getOID();
#if 0
  for (auto callback : block->_callbacks)
    callback->inDbBTermPostConnect((dbBTerm*) this);
#endif
}

void _dbModITerm::disconnectNet(_dbModITerm* moditerm, _dbBlock* block)
{
  // unlink moditerm from the net
#if 0
  for (auto callback : block->_callbacks)
    callback->inDbBTermPreDisconnect((dbBTerm*) this);
#endif
  _dbModNet* net = block->_modnet_tbl->getPtr(moditerm->_net);
  uint id = moditerm->getOID();

  if (net->_moditerms == id) {
    net->_moditerms = moditerm->_next_moditerm;

    if (net->_moditerms != 0) {
      _dbModITerm* it = block->_moditerm_tbl->getPtr(net->_moditerms);
      it->_prev_moditerm = 0;
    }
  } else {
    if (moditerm->_next_moditerm != 0) {
      _dbModITerm* next
          = block->_moditerm_tbl->getPtr(moditerm->_next_moditerm);
      next->_prev_moditerm = moditerm->_prev_moditerm;
    }

    if (moditerm->_prev_moditerm != 0) {
      _dbModITerm* prev
          = block->_moditerm_tbl->getPtr(moditerm->_prev_moditerm);
      prev->_next_moditerm = moditerm->_next_moditerm;
    }
  }
  _net = 0;

#if 0
  for (auto callback : block->_callbacks)
    callback->inDbBTermPostDisConnect((dbBTerm*) this, (dbNet*) net);
#endif
}

// User Code End dbModITermPublicMethods
}  // namespace odb
   // Generator Code End Cpp
