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
// User Code End Includes
namespace odb {

template class dbTable<_dbModTerm>;

bool _dbModTerm::operator==(const _dbModTerm& rhs) const
{
  if (_name != rhs._name)
    return false;

  if (_next_entry != rhs._next_entry)
    return false;

  if (_parent != rhs._parent)
    return false;

  if (_module_next != rhs._module_next)
    return false;

  // User Code Begin ==
  // User Code End ==
  return true;
}
bool _dbModTerm::operator<(const _dbModTerm& rhs) const
{
  // User Code Begin <
  // User Code End <
  return true;
}
void _dbModTerm::differences(dbDiff& diff,
                             const char* field,
                             const _dbModTerm& rhs) const
{
  DIFF_BEGIN

  DIFF_FIELD(_name);
  DIFF_FIELD(_next_entry);
  DIFF_FIELD(_parent);
  DIFF_FIELD(_module_next);
  // User Code Begin Differences
  // User Code End Differences
  DIFF_END
}
void _dbModTerm::out(dbDiff& diff, char side, const char* field) const
{
  DIFF_OUT_BEGIN
  DIFF_OUT_FIELD(_name);
  DIFF_OUT_FIELD(_next_entry);
  DIFF_OUT_FIELD(_parent);
  DIFF_OUT_FIELD(_module_next);

  // User Code Begin Out
  // User Code End Out
  DIFF_END
}
_dbModTerm::_dbModTerm(_dbDatabase* db)
{
  // User Code Begin Constructor
  // User Code End Constructor
}
_dbModTerm::_dbModTerm(_dbDatabase* db, const _dbModTerm& r)
{
  _name = r._name;
  _next_entry = r._next_entry;
  _parent = r._parent;
  _module_next = r._module_next;
  // User Code Begin CopyConstructor
  // User Code End CopyConstructor
}

dbIStream& operator>>(dbIStream& stream, _dbModTerm& obj)
{
  stream >> obj._name;
  stream >> obj._next_entry;
  stream >> obj._parent;
  stream >> obj._module_next;
  // User Code Begin >>
  // User Code End >>
  return stream;
}
dbOStream& operator<<(dbOStream& stream, const _dbModTerm& obj)
{
  stream << obj._name;
  stream << obj._next_entry;
  stream << obj._parent;
  stream << obj._module_next;
  // User Code Begin <<
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
                             const char* name)
{
  _dbModule* parent = (_dbModule*) parentModule;
  _dbBlock* block = (_dbBlock*) parent->getOwner();
  std::string h_name = std::string(parent->_name) + '/' + std::string(name);
  if (block->_modterm_hash.hasMember(h_name.c_str()))
    return nullptr;
  _dbModTerm* modterm = block->_modterm_tbl->create();
  modterm->_name = strdup(h_name.c_str());
  ZALLOCATED(modterm->_name);
  modterm->_parent = parent->getOID();
  modterm->_module_next = parent->_modterms;
  parent->_modterms = modterm->getOID();
  block->_modterm_hash.insert(modterm);
  return (dbModTerm*) modterm;
}

// User Code End dbModTermPublicMethods
}  // namespace odb
   // Generator Code End Cpp
