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
#include "dbModInstHeader.h"

#include "db.h"
#include "dbBlock.h"
#include "dbDatabase.h"
#include "dbDiff.hpp"
#include "dbModule.h"
#include "dbTable.h"
#include "dbTable.hpp"
// User Code Begin Includes
// User Code End Includes
namespace odb {

template class dbTable<_dbModInstHeader>;

bool _dbModInstHeader::operator==(const _dbModInstHeader& rhs) const
{
  if (_next_entry != rhs._next_entry)
    return false;

  if (_master != rhs._master)
    return false;

  // User Code Begin ==
  // User Code End ==
  return true;
}
bool _dbModInstHeader::operator<(const _dbModInstHeader& rhs) const
{
  // User Code Begin <
  // User Code End <
  return true;
}
void _dbModInstHeader::differences(dbDiff& diff,
                                   const char* field,
                                   const _dbModInstHeader& rhs) const
{
  DIFF_BEGIN

  DIFF_FIELD(_next_entry);
  DIFF_FIELD(_master);
  // User Code Begin Differences
  // User Code End Differences
  DIFF_END
}
void _dbModInstHeader::out(dbDiff& diff, char side, const char* field) const
{
  DIFF_OUT_BEGIN
  DIFF_OUT_FIELD(_next_entry);
  DIFF_OUT_FIELD(_master);

  // User Code Begin Out
  // User Code End Out
  DIFF_END
}
_dbModInstHeader::_dbModInstHeader(_dbDatabase* db)
{
  // User Code Begin Constructor
  // User Code End Constructor
}
_dbModInstHeader::_dbModInstHeader(_dbDatabase* db, const _dbModInstHeader& r)
{
  _next_entry = r._next_entry;
  _master = r._master;
  // User Code Begin CopyConstructor
  // User Code End CopyConstructor
}

dbIStream& operator>>(dbIStream& stream, _dbModInstHeader& obj)
{
  stream >> obj._next_entry;
  stream >> obj._master;
  stream >> obj._modterms;
  // User Code Begin >>
  // User Code End >>
  return stream;
}
dbOStream& operator<<(dbOStream& stream, const _dbModInstHeader& obj)
{
  stream << obj._next_entry;
  stream << obj._master;
  stream << obj._modterms;
  // User Code Begin <<
  // User Code End <<
  return stream;
}

_dbModInstHeader::~_dbModInstHeader()
{
  // User Code Begin Destructor
  // User Code End Destructor
}

// User Code Begin PrivateMethods
// User Code End PrivateMethods

////////////////////////////////////////////////////////////////////
//
// dbModInstHeader - Methods
//
////////////////////////////////////////////////////////////////////

dbModule* dbModInstHeader::getMaster() const
{
  _dbModInstHeader* obj = (_dbModInstHeader*) this;
  if (obj->_master == 0)
    return NULL;
  _dbBlock* par = (_dbBlock*) obj->getOwner();
  return (dbModule*) par->_module_tbl->getPtr(obj->_master);
}

// User Code Begin dbModInstHeaderPublicMethods
// User Code End dbModInstHeaderPublicMethods
}  // namespace odb
   // Generator Code End Cpp