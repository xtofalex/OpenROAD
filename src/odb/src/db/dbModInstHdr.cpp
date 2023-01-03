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
#include "dbModInstHdr.h"

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

template class dbTable<_dbModInstHdr>;

bool _dbModInstHdr::operator==(const _dbModInstHdr& rhs) const
{
  if (_id != rhs._id)
    return false;

  if (_next_entry != rhs._next_entry)
    return false;

  if (_master != rhs._master)
    return false;

  // User Code Begin ==
  // User Code End ==
  return true;
}
bool _dbModInstHdr::operator<(const _dbModInstHdr& rhs) const
{
  // User Code Begin <
  // User Code End <
  return true;
}
void _dbModInstHdr::differences(dbDiff& diff,
                                const char* field,
                                const _dbModInstHdr& rhs) const
{
  DIFF_BEGIN

  DIFF_FIELD(_id);
  DIFF_FIELD(_next_entry);
  DIFF_FIELD(_master);
  // User Code Begin Differences
  // User Code End Differences
  DIFF_END
}
void _dbModInstHdr::out(dbDiff& diff, char side, const char* field) const
{
  DIFF_OUT_BEGIN
  DIFF_OUT_FIELD(_id);
  DIFF_OUT_FIELD(_next_entry);
  DIFF_OUT_FIELD(_master);

  // User Code Begin Out
  // User Code End Out
  DIFF_END
}
_dbModInstHdr::_dbModInstHdr(_dbDatabase* db)
{
  // User Code Begin Constructor
  _id = 0;
  // User Code End Constructor
}
_dbModInstHdr::_dbModInstHdr(_dbDatabase* db, const _dbModInstHdr& r)
{
  _id = r._id;
  _next_entry = r._next_entry;
  _master = r._master;
  // User Code Begin CopyConstructor
  // User Code End CopyConstructor
}

dbIStream& operator>>(dbIStream& stream, _dbModInstHdr& obj)
{
  stream >> obj._id;
  stream >> obj._next_entry;
  stream >> obj._master;
  stream >> obj._modterms;
  // User Code Begin >>
  // User Code End >>
  return stream;
}
dbOStream& operator<<(dbOStream& stream, const _dbModInstHdr& obj)
{
  stream << obj._id;
  stream << obj._next_entry;
  stream << obj._master;
  stream << obj._modterms;
  // User Code Begin <<
  // User Code End <<
  return stream;
}

_dbModInstHdr::~_dbModInstHdr()
{
  // User Code Begin Destructor
  // User Code End Destructor
}

// User Code Begin PrivateMethods
// User Code End PrivateMethods

////////////////////////////////////////////////////////////////////
//
// dbModInstHdr - Methods
//
////////////////////////////////////////////////////////////////////

dbModule* dbModInstHdr::getMaster() const
{
  _dbModInstHdr* obj = (_dbModInstHdr*) this;
  if (obj->_master == 0)
    return NULL;
  _dbBlock* par = (_dbBlock*) obj->getOwner();
  return (dbModule*) par->_module_tbl->getPtr(obj->_master);
}

// User Code Begin dbModInstHdrPublicMethods
dbModInstHdr* dbModInstHdr::create(dbBlock* block_, dbModule* masterModule_) {
  _dbBlock* block = (_dbBlock*) block_;
  _dbModule* masterModule = (_dbModule*) masterModule_;

  if (block->_modinst_hdr_hash.hasMember(masterModule->_id))
    return NULL;

  _dbModInstHdr* inst_hdr;
  // initialize the inst_hdr structure
  inst_hdr = block->_modinst_hdr_tbl->create();
  inst_hdr->_id = masterModule->_id;
  inst_hdr->_master = masterModule->getOID();

  // insert the inst_hdr into the block modinst_hdr hash table.
  block->_modinst_hdr_hash.insert(inst_hdr);

  //
  // Copied from dbInstHdr.cpp
  // Each ITerm of and instances points back the MTerm the ITerm
  // represents. To save space in the ITerm and to make the order
  // of the ITerm of an instance appear as {output, inout, input},
  // a mapping structure is used. This structure maps offset of an ITerm
  // back to MTerm. The mapping vector is ordered as {output, inout, input}.
  // The alternative to this strategy would be to enforce a create order
  // on the creation of MTerms on a Master. This stragegy complicates the
  // creation of MTerms with streamed input formats such as LEF, because
  // one does not know the order of MTerms as they would be created.
  // Consequently, you would need to buffer the data of a master, i.e., a LEF
  // MACRO until the complete MACRO is parsed...
  //
  uint modterm_cnt = masterModule_->getTerms().size();
  inst_hdr->_modterms.resize(modterm_cnt);

  // modterms, this set is ordered: {output, inout, input}
  dbSet<dbModTerm> modterms = masterModule_->getTerms();

  dbSet<dbModTerm>::iterator itr;
  int i = 0;
  for (itr = modterms.begin(); itr != modterms.end(); ++itr) {
    dbModTerm* modterm = *itr;
    inst_hdr->_modterms[i++] = modterm->getImpl()->getOID();
  }

  return (dbModInstHdr*) inst_hdr;
}
// User Code End dbModInstHdrPublicMethods
}  // namespace odb
   // Generator Code End Cpp
