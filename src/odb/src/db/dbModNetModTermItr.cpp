///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2019, Nefelus Inc
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
#include "dbModNetModTermItr.h"

#include "dbModNet.h"
#include "dbModTerm.h"
#include "dbTable.h"
// User Code Begin Includes
// User Code End Includes

namespace odb {

////////////////////////////////////////////////////////////////////
//
// dbModNetModTermItr - Methods
//
////////////////////////////////////////////////////////////////////

bool dbModNetModTermItr::reversible()
{
  return true;
}

bool dbModNetModTermItr::orderReversed()
{
  return true;
}

void dbModNetModTermItr::reverse(dbObject* parent)
{
  // User Code Begin reverse
  _dbModNet* net = (_dbModNet*) parent;
  uint id = net->_modterms;
  uint list = 0;

  while (id != 0) {
    _dbModTerm* modterm = _modterm_tbl->getPtr(id);
    uint n = modterm->_next_modterm;
    modterm->_next_modterm = list;
    list = id;
    id = n;
  }

  uint prev = 0;
  id = list;

  while (id != 0) {
    _dbModTerm* modterm = _modterm_tbl->getPtr(id);
    modterm->_prev_modterm = prev;
    prev = id;
    id = modterm->_next_modterm;
  }

  net->_modterms = list;
  // User Code End reverse
}

uint dbModNetModTermItr::sequential()
{
  return 0;
}

uint dbModNetModTermItr::size(dbObject* parent)
{
  uint id;
  uint cnt = 0;

  for (id = dbModNetModTermItr::begin(parent);
       id != dbModNetModTermItr::end(parent);
       id = dbModNetModTermItr::next(id))
    ++cnt;

  return cnt;
}

uint dbModNetModTermItr::begin(dbObject* parent)
{
  // User Code Begin begin
  _dbModNet* net = (_dbModNet*) parent;
  return net->_modterms;
  // User Code End begin
}

uint dbModNetModTermItr::end(dbObject* /* unused: parent */)
{
  return 0;
}

uint dbModNetModTermItr::next(uint id, ...)
{
  // User Code Begin next
  _dbModTerm* modterm = _modterm_tbl->getPtr(id);
  return modterm->_next_modterm;
  // User Code End next
}

dbObject* dbModNetModTermItr::getObject(uint id, ...)
{
  return _modterm_tbl->getPtr(id);
}
// User Code Begin Methods
// User Code End Methods
}  // namespace odb
   // Generator Code End Cpp
