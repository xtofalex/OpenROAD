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
#include "dbModNet.h"

#include "db.h"
#include "dbBlock.h"
#include "dbDatabase.h"
#include "dbDiff.hpp"
#include "dbHashTable.hpp"
#include "dbModNetModITermItr.h"
#include "dbModNetModTermItr.h"
#include "dbModule.h"
#include "dbTable.h"
#include "dbTable.hpp"
// User Code Begin Includes
// User Code End Includes
namespace odb {

template class dbTable<_dbModNet>;

bool _dbModNet::operator==(const _dbModNet& rhs) const
{
  if (_name != rhs._name)
    return false;

  if (_next_entry != rhs._next_entry)
    return false;

  if (_modterms != rhs._modterms)
    return false;

  if (_moditerms != rhs._moditerms)
    return false;

  if (_parent != rhs._parent)
    return false;

  if (_module_next != rhs._module_next)
    return false;

  // User Code Begin ==
  // User Code End ==
  return true;
}
bool _dbModNet::operator<(const _dbModNet& rhs) const
{
  // User Code Begin <
  if (strcmp(_name, rhs._name) >= 0)
    return false;
  // User Code End <
  return true;
}
void _dbModNet::differences(dbDiff& diff,
                            const char* field,
                            const _dbModNet& rhs) const
{
  DIFF_BEGIN

  DIFF_FIELD(_name);
  DIFF_FIELD(_next_entry);
  DIFF_FIELD(_modterms);
  DIFF_FIELD(_moditerms);
  DIFF_FIELD(_parent);
  DIFF_FIELD(_module_next);
  // User Code Begin Differences
  // User Code End Differences
  DIFF_END
}
void _dbModNet::out(dbDiff& diff, char side, const char* field) const
{
  DIFF_OUT_BEGIN
  DIFF_OUT_FIELD(_name);
  DIFF_OUT_FIELD(_next_entry);
  DIFF_OUT_FIELD(_modterms);
  DIFF_OUT_FIELD(_moditerms);
  DIFF_OUT_FIELD(_parent);
  DIFF_OUT_FIELD(_module_next);

  // User Code Begin Out
  // User Code End Out
  DIFF_END
}
_dbModNet::_dbModNet(_dbDatabase* db)
{
  // User Code Begin Constructor
  _name = 0;
  _next_entry = 0;
  _parent = 0;
  _module_next = 0;
  // User Code End Constructor
}
_dbModNet::_dbModNet(_dbDatabase* db, const _dbModNet& r)
{
  _name = r._name;
  _next_entry = r._next_entry;
  _modterms = r._modterms;
  _moditerms = r._moditerms;
  _parent = r._parent;
  _module_next = r._module_next;
  // User Code Begin CopyConstructor
  // User Code End CopyConstructor
}

dbIStream& operator>>(dbIStream& stream, _dbModNet& obj)
{
  stream >> obj._name;
  stream >> obj._next_entry;
  stream >> obj._modterms;
  stream >> obj._moditerms;
  stream >> obj._parent;
  stream >> obj._module_next;
  // User Code Begin >>
  // User Code End >>
  return stream;
}
dbOStream& operator<<(dbOStream& stream, const _dbModNet& obj)
{
  stream << obj._name;
  stream << obj._next_entry;
  stream << obj._modterms;
  stream << obj._moditerms;
  stream << obj._parent;
  stream << obj._module_next;
  // User Code Begin <<
  // User Code End <<
  return stream;
}

_dbModNet::~_dbModNet()
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
// dbModNet - Methods
//
////////////////////////////////////////////////////////////////////

dbModule* dbModNet::getParent() const
{
  _dbModNet* obj = (_dbModNet*) this;
  if (obj->_parent == 0)
    return NULL;
  _dbBlock* par = (_dbBlock*) obj->getOwner();
  return (dbModule*) par->_module_tbl->getPtr(obj->_parent);
}

// User Code Begin dbModNetPublicMethods
dbModNet* dbModNet::create(dbModule* parentModule, const char* name)
{
  _dbModule* parent = (_dbModule*) parentModule;
  _dbBlock* block = (_dbBlock*) parent->getOwner();
  std::string h_name = std::string(parent->_name) + '/' + std::string(name);
  if (block->_modnet_hash.hasMember(h_name.c_str()))
    return nullptr;
  _dbModNet* modnet = block->_modnet_tbl->create();
  modnet->_name = strdup(h_name.c_str());
  ZALLOCATED(modnet->_name);
  modnet->_parent = parent->getOID();
  modnet->_module_next = parent->_modnets;
  parent->_modnets = modnet->getOID();
  block->_modnet_hash.insert(modnet);
  return (dbModNet*) modnet;
}

void dbModNet::destroy(dbModNet* modnet_)
{
  _dbModNet* modnet = (_dbModNet*) modnet_;
  _dbBlock* block = (_dbBlock*) modnet->getOwner();

  block->_modnet_hash.remove(modnet);
  dbProperty::destroyProperties(modnet);
  block->_modnet_tbl->destroy(modnet);
}

dbSet<dbModNet>::iterator dbModNet::destroy(dbSet<dbModNet>::iterator& itr)
{
  dbModNet* modnet = *itr;
  dbSet<dbModNet>::iterator next = ++itr;
  destroy(modnet);
  return next;
}

std::string dbModNet::getName() const
{
  _dbModNet* obj = (_dbModNet*) this;
  std::string h_name = std::string(obj->_name);
  size_t idx = h_name.find_last_of('/');
  return h_name.substr(idx + 1);
}

dbSet<dbModTerm> dbModNet::getTerms()
{
  _dbModNet* net = (_dbModNet*) this;
  _dbBlock* block = (_dbBlock*) net->getOwner();
  return dbSet<dbModTerm>(net, block->_modnet_modterm_itr);
}

dbSet<dbModITerm> dbModNet::getITerms()
{
  _dbModNet* net = (_dbModNet*) this;
  _dbBlock* block = (_dbBlock*) net->getOwner();
  return dbSet<dbModITerm>(net, block->_modnet_moditerm_itr);
}

// User Code End dbModNetPublicMethods
}  // namespace odb
   // Generator Code End Cpp
