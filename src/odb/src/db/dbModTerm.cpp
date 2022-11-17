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
#include "dbModTerm.h"

#include "db.h"
#include "dbBlock.h"
#include "dbDatabase.h"
#include "dbDiff.hpp"
#include "dbHashTable.hpp"
#include "dbModule.h"
#include "dbTable.h"
#include "dbTable.hpp"
// User Code Begin Includes
#include "dbModNet.h"
// User Code End Includes
namespace odb {

template class dbTable<_dbModTerm>;

bool _dbModTerm::operator==(const _dbModTerm& rhs) const
{
  if (_name != rhs._name)
    return false;

  if (_net != rhs._net)
    return false;

  if (_next_entry != rhs._next_entry)
    return false;

  if (_parent != rhs._parent)
    return false;

  if (_module_next != rhs._module_next)
    return false;

  // User Code Begin ==
  if (_flags._io_type != rhs._flags._io_type)
    return false;
  // User Code End ==
  return true;
}
bool _dbModTerm::operator<(const _dbModTerm& rhs) const
{
  // User Code Begin <
  if (strcmp(_name, rhs._name) >= 0)
    return false;
  // User Code End <
  return true;
}
void _dbModTerm::differences(dbDiff& diff,
                             const char* field,
                             const _dbModTerm& rhs) const
{
  DIFF_BEGIN

  DIFF_FIELD(_name);
  DIFF_FIELD(_net);
  DIFF_FIELD(_next_entry);
  DIFF_FIELD(_parent);
  DIFF_FIELD(_module_next);
  // User Code Begin Differences
  DIFF_FIELD(_flags._io_type);
  // User Code End Differences
  DIFF_END
}
void _dbModTerm::out(dbDiff& diff, char side, const char* field) const
{
  DIFF_OUT_BEGIN
  DIFF_OUT_FIELD(_name);
  DIFF_OUT_FIELD(_net);
  DIFF_OUT_FIELD(_next_entry);
  DIFF_OUT_FIELD(_parent);
  DIFF_OUT_FIELD(_module_next);

  // User Code Begin Out
  DIFF_OUT_FIELD(_flags._io_type);
  // User Code End Out
  DIFF_END
}
_dbModTerm::_dbModTerm(_dbDatabase* db)
{
  // User Code Begin Constructor
  _name = 0;
  _next_entry = 0;
  _parent = 0;
  _module_next = 0;
  _flags._io_type = dbIoType::INPUT;
  // User Code End Constructor
}
_dbModTerm::_dbModTerm(_dbDatabase* db, const _dbModTerm& r)
{
  _name = r._name;
  _net = r._net;
  _next_entry = r._next_entry;
  _parent = r._parent;
  _module_next = r._module_next;
  // User Code Begin CopyConstructor
  _flags._io_type = r._flags._io_type;
  // User Code End CopyConstructor
}

dbIStream& operator>>(dbIStream& stream, _dbModTerm& obj)
{
  stream >> obj._name;
  stream >> obj._net;
  stream >> obj._next_entry;
  stream >> obj._parent;
  stream >> obj._module_next;
  // User Code Begin >>
  uint* bit_field = (uint*) &obj._flags;
  stream >> *bit_field;
  // User Code End >>
  return stream;
}
dbOStream& operator<<(dbOStream& stream, const _dbModTerm& obj)
{
  stream << obj._name;
  stream << obj._net;
  stream << obj._next_entry;
  stream << obj._parent;
  stream << obj._module_next;
  // User Code Begin <<
  uint* bit_field = (uint*) &obj._flags;
  stream << *bit_field;
  // User Code End <<
  return stream;
}

_dbModTerm::~_dbModTerm()
{
  if (_name)
    free((void*) _name);
  // User Code Begin Destructor
  // User Code End Destructor
}

// User Code Begin PrivateMethods
// User Code End PrivateMethods

////////////////////////////////////////////////////////////////////
//
// dbModTerm - Methods
//
////////////////////////////////////////////////////////////////////

dbModule* dbModTerm::getParent() const
{
  _dbModTerm* obj = (_dbModTerm*) this;
  if (obj->_parent == 0)
    return NULL;
  _dbBlock* par = (_dbBlock*) obj->getOwner();
  return (dbModule*) par->_module_tbl->getPtr(obj->_parent);
}

// User Code Begin dbModTermPublicMethods
dbModTerm* dbModTerm::create(dbModule* parentModule,
                             const char* name,
                             dbIoType ioType)
{
  _dbModule* parent = (_dbModule*) parentModule;
  _dbBlock* block = (_dbBlock*) parent->getOwner();
  std::string h_name = std::string(parent->_name) + '/' + std::string(name);
  if (block->_modterm_hash.hasMember(h_name.c_str()))
    return nullptr;
  _dbModTerm* modterm = block->_modterm_tbl->create();
  modterm->_name = strdup(h_name.c_str());
  ZALLOCATED(modterm->_name);
  modterm->_flags._io_type = ioType;
  modterm->_parent = parent->getOID();
  modterm->_module_next = parent->_modterms;
  parent->_modterms = modterm->getOID();
  block->_modterm_hash.insert(modterm);
  return (dbModTerm*) modterm;
}

std::string dbModTerm::getName() const
{
  _dbModTerm* obj = (_dbModTerm*) this;
  std::string h_name = std::string(obj->_name);
  size_t idx = h_name.find_last_of('/');
  return h_name.substr(idx + 1);
}

dbIoType dbModTerm::getIoType() const
{
  _dbModTerm* mterm = (_dbModTerm*) this;
  return dbIoType(mterm->_flags._io_type);
}

dbModNet* dbModTerm::getNet()
{
  _dbModTerm* modTerm = (_dbModTerm*) this;
  if (modTerm->_net) {
    _dbModule* parent = (_dbModule*) getParent();
    _dbBlock* block = (_dbBlock*) parent->getOwner();
    _dbModNet* net = block->_modnet_tbl->getPtr(modTerm->_net);
    return (dbModNet*) net;
  } else
    return nullptr;
}

void dbModTerm::disconnect() {
}

void dbModTerm::connect(dbModNet* net) {
}

// User Code End dbModTermPublicMethods
}  // namespace odb
   // Generator Code End Cpp
