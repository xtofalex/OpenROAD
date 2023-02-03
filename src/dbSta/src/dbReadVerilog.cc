/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019, The Regents of the University of California
// All rights reserved.
//
// BSD 3-Clause License
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
//
///////////////////////////////////////////////////////////////////////////////

#include "db_sta/dbReadVerilog.hh"

#include <map>
#include <string>
#include <iostream>

#include "db_sta/dbNetwork.hh"
#include "odb/db.h"
#include "ord/OpenRoad.hh"
#include "sta/ConcreteNetwork.hh"
#include "sta/NetworkCmp.hh"
#include "sta/PortDirection.hh"
#include "sta/Vector.hh"
#include "sta/VerilogReader.hh"
#include "utl/Logger.h"

namespace ord {

using odb::dbBlock;
using odb::dbBTerm;
using odb::dbChip;
using odb::dbDatabase;
using odb::dbInst;
using odb::dbIoType;
using odb::dbITerm;
using odb::dbLib;
using odb::dbMaster;
using odb::dbModInst;
using odb::dbModule;
using odb::dbModITerm;
using odb::dbModTerm;
using odb::dbModNet;
using odb::dbMTerm;
using odb::dbNet;
using odb::dbSet;
using odb::dbTech;
using utl::ORD;

using sta::Cell;
using sta::CellPortBitIterator;
using sta::CellPortIterator;
using sta::ConcreteNetwork;
using sta::ConnectedPinIterator;
using sta::dbNetwork;
using sta::deleteVerilogReader;
using sta::Instance;
using sta::InstanceChildIterator;
using sta::InstancePinIterator;
using sta::LeafInstanceIterator;
using sta::LibertyCell;
using sta::Library;
using sta::Net;
using sta::NetConnectedPinIterator;
using sta::NetPinIterator;
using sta::NetIterator;
using sta::NetTermIterator;
using sta::Network;
using sta::NetworkReader;
using sta::Pin;
using sta::PinPathNameLess;
using sta::PinSeq;
using sta::Port;
using sta::PortDirection;
using sta::Term;

using utl::Logger;
using utl::STA;

#define DEBUG 0

dbVerilogNetwork::dbVerilogNetwork() :
  ConcreteNetwork(), db_network_(nullptr)
{
  report_ = nullptr;
  debug_ = nullptr;
}

void
dbVerilogNetwork::init(dbNetwork* db_network)
{
  db_network_ = db_network;
  copyState(db_network_);
}

dbVerilogNetwork*
makeDbVerilogNetwork()
{
  return new dbVerilogNetwork;
}

void
initDbVerilogNetwork(ord::OpenRoad* openroad)
{
  openroad->getVerilogNetwork()->init(openroad->getDbNetwork());
}

void
deleteDbVerilogNetwork(dbVerilogNetwork* verilog_network)
{
  delete verilog_network;
}

// Facade that looks in the db network for a liberty cell if
// there isn't one in the verilog network.
Cell*
dbVerilogNetwork::findAnyCell(const char* name)
{
  Cell* cell = ConcreteNetwork::findAnyCell(name);
  if (cell == nullptr)
    cell = db_network_->findAnyCell(name);
  return cell;
}

void
dbReadVerilog(const char* filename, dbVerilogNetwork* verilog_network)
{
  sta::readVerilogFile(filename, verilog_network);
}

////////////////////////////////////////////////////////////////

class Verilog2db
{
public:
  Verilog2db(Network* verilog_network, dbDatabase* db, Logger* logger);
  void makeBlock();
  void makeDbNetlist();

protected:
  void makeDbModule(Instance* inst, dbModule* parent);
  dbIoType staToDb(PortDirection* dir);
  void recordBusPortsOrder();
  void makeDbNets(const Instance* inst);
  bool hasTerminals(Net* net) const;
  dbMaster* getMaster(Cell* cell);
  dbModule* makeUniqueDbModule(const char* name);
  void makeDbModuleTerms(dbModule* module, const Cell* cell);
  void makeDbModuleNets(dbModule* module, const Instance* instance);

  Network* network_;
  dbDatabase* db_;
  dbBlock* block_;
  Logger* logger_;
  std::map<Cell*, dbMaster*> master_map_;
  std::map<std::string, int> uniquify_id_;  // key: module name
};

void
dbLinkDesign(const char* top_cell_name,
             dbVerilogNetwork* verilog_network,
             dbDatabase* db,
             Logger* logger)
{
  bool link_make_black_boxes = true;
  bool success = verilog_network->linkNetwork(
      top_cell_name,
      link_make_black_boxes,
      verilog_network->report());
  if (success) {
    Verilog2db v2db(verilog_network, db, logger);
    v2db.makeBlock();
    v2db.makeDbNetlist();
    deleteVerilogReader();
  }
}

Verilog2db::Verilog2db(Network* network, dbDatabase* db, Logger* logger) :
  network_(network), db_(db), block_(nullptr), logger_(logger)
{
}

void
Verilog2db::makeBlock()
{
  dbChip* chip = db_->getChip();
  if (chip == nullptr)
    chip = dbChip::create(db_);
  block_ = chip->getBlock();
  if (block_) {
    // Delete existing db network objects.
    auto insts = block_->getInsts();
    for (auto iter = insts.begin(); iter != insts.end();) {
      iter = dbInst::destroy(iter);
    }
    auto nets = block_->getNets();
    for (auto iter = nets.begin(); iter != nets.end();) {
      iter = dbNet::destroy(iter);
    }
    auto bterms = block_->getBTerms();
    for (auto iter = bterms.begin(); iter != bterms.end();) {
      iter = dbBTerm::destroy(iter);
    }
    auto mod_terms = block_->getModTerms();
    for (auto iter = mod_terms.begin(); iter != mod_terms.end();) {
      iter = dbModTerm::destroy(iter);
    }
    auto mod_nets = block_->getModNets();
    for (auto iter = mod_nets.begin(); iter != mod_nets.end();) {
      iter = dbModNet::destroy(iter);
    }
    auto mod_insts = block_->getTopModule()->getChildren();
    for (auto iter = mod_insts.begin(); iter != mod_insts.end();) {
      iter = dbModInst::destroy(iter);
    }
  }
  else {
    const char* design = network_->name(network_->cell(network_->topInstance()));
    block_ = dbBlock::create(chip, design, network_->pathDivider());
  }
  dbTech* tech = db_->getTech();
  block_->setDefUnits(tech->getLefUnits());
  block_->setBusDelimeters('[', ']');
}

void
Verilog2db::makeDbNetlist()
{
  recordBusPortsOrder();
  makeDbModule(network_->topInstance(), /* parent */ nullptr);
  makeDbNets(network_->topInstance());
}

void
Verilog2db::recordBusPortsOrder()
{
  // OpenDB does not have any concept of bus ports.
  // Use a property to annotate the bus names as msb or lsb first for writing
  // verilog.
  Cell* top_cell = network_->cell(network_->topInstance());
  CellPortIterator* bus_iter = network_->portIterator(top_cell);
  while (bus_iter->hasNext()) {
    Port* port = bus_iter->next();
    if (network_->isBus(port)) {
      const char* port_name = network_->name(port);
      int from = network_->fromIndex(port);
      int to = network_->toIndex(port);
      string key = "bus_msb_first ";
      key += port_name;
      odb::dbBoolProperty::create(block_, key.c_str(), from > to);
    }
  }
  delete bus_iter;
}

void
Verilog2db::makeDbModuleTerms(dbModule* module, const Cell* cell) {
#if DEBUG
  std::cerr << "makeDbModuleTerms: " << module->getName() << " : " << network_->name(cell) << std::endl; 
#endif
  CellPortIterator* port_iter = network_->portIterator(cell);
  while (port_iter->hasNext()) {
    Port* port = port_iter->next();
    const char* port_name = network_->name(port);
    dbIoType io_type = staToDb(network_->direction(port));
    if (network_->isBus(port)) {
      int from = network_->fromIndex(port);
      int to = network_->toIndex(port);
      int incr = (from < to)?1:-1;
      int end = (from < to)?to+1:to-1;
      for (int bit = from; bit != end; bit+=incr) {
        std::string port_name_str = std::string(port_name) + '[' + std::to_string(bit) + ']';
        auto term = dbModTerm::create(module, port_name_str.c_str(), io_type);
#if DEBUG
        std::cerr << "makeDbModuleTerms: " << term->getName() << std::endl; 
#endif
      }
    } else {
      auto term = dbModTerm::create(module, port_name, io_type);
#if DEBUG
      std::cerr << "makeDbModuleTerms: " << term->getName() << std::endl; 
#endif
    }
  }
  delete port_iter;
}

void
Verilog2db::makeDbModuleNets(dbModule* module, const Instance* instance) {
#if DEBUG
  std::cerr << "makeDbModuleNets : " << module->getName() << " instance: " << network_->pathName(instance) << std::endl;
#endif
  NetIterator* net_iter = network_->netIterator(instance);
  while (net_iter->hasNext()) {
    Net* net = net_iter->next();
    const char* net_name = network_->pathName(net);
    dbModNet* mod_net = dbModNet::create(module, net_name); 
#if DEBUG
    std::cerr << "makeDbModuleNet: " << mod_net->getName() << std::endl; 
#endif
    //if (network_->isPower(net))
    //  mod_net->setSigType(odb::dbSigType::POWER);
    //if (network_->isGround(net))
    //  mod_net->setSigType(odb::dbSigType::GROUND);
#if 0
    if (is_top || !hasTerminals(net)) {
      dbNet* db_net = dbNet::create(block_, net_name);

#endif

    NetTermIterator* term_iter = network_->termIterator(net);
    while (term_iter->hasNext()) {
      Term* term = term_iter->next();
#if DEBUG
      std::cerr << "Term: " << network_->portName(term) << std::endl;
#endif
      const char* term_name = network_->portName(term);
      dbModTerm* mod_term = module->findModTerm(term_name);
      if (not term) {
        std::cerr << "cannot find " << term_name << " in " << module->getName() << std::endl;
        std::cerr << "Terms are: " << std::endl;
        for (auto term: module->getTerms()) {
          std::cerr << term->getName() << std::endl;
        }
      }
      assert(term);
      mod_term->connect(mod_net);
    }
    delete term_iter;

    // Sort connected pins for regression stability.
    PinSeq net_pins;
    NetPinIterator* pin_iter = network_->pinIterator(net);
    while (pin_iter->hasNext()) {
      Pin* pin = pin_iter->next();
#if DEBUG
      std::cerr << "Pin " << network_->portName(pin) << std::endl;
#endif
      Instance* instance = network_->instance(pin);
#if 0
      std::cerr << "Instance " << network_->pathName(instance) << std::endl;
#endif
      net_pins.push_back(pin);
    }
    delete pin_iter;
    sort(net_pins, PinPathNameLess(network_));

    for (Pin* pin : net_pins) {
      const char* port_name = network_->portName(pin);
      Instance* inst = network_->instance(pin);
      assert(inst);
      const char* inst_name = network_->name(inst);
      assert(inst_name);
      dbModInst* mod_inst = module->findModInst(inst_name);
      if (not mod_inst) {
        std::cerr << "cannot find " << inst_name << " in " << module->getName() << std::endl;
        std::cerr << "instances are: " << std::endl;
        for (auto instance: module->getChildren()) {
          std::cerr << instance->getName() << std::endl;
        }
      }
      assert(mod_inst);
      dbModITerm* iterm = mod_inst->findITerm(port_name);
      assert(iterm);
      iterm->connect(mod_net);
    }
  }
  delete net_iter;
}

dbModule*
Verilog2db::makeUniqueDbModule(const char* name)
{
  dbModule* module;
  do {
    std::string full_name(name);
    int& id = uniquify_id_[name];
    if (id > 0) {
      full_name += '-' + std::to_string(id);
    }
    ++id;
    module = dbModule::create(block_, full_name.c_str());
  } while (module == nullptr);
  return module;
}

// Recursively builds odb's dbModule/dbModInst hierarchy corresponding
// to the sta network rooted at inst.  parent is the dbModule to build
// the hierarchy under. If null the top module is used.

void
Verilog2db::makeDbModule(Instance* inst, dbModule* parent)
{
  Cell* cell = network_->cell(inst);

  dbModule* module;
  if (parent == nullptr) {
    module = block_->getTopModule();
    makeDbModuleTerms(module, cell);
  }
  else {
    module = makeUniqueDbModule(network_->name(cell));
    //create mod terms
    makeDbModuleTerms(module, cell);
    //
    dbModInst* modinst = dbModInst::create(parent, module, network_->name(inst));
#if 0
    std::cerr << "In : " << parent->getName() << std::endl;
    std::cerr << "Created " << modinst->getName() << std::endl;
#endif
    if (modinst == nullptr) {
      logger_->warn(ORD,
                    1014,
                    "hierachical instance creation failed for {} of {}",
                    network_->name(inst),
                    network_->name(cell));
      return;
    }
  }
  InstanceChildIterator* child_iter = network_->childIterator(inst);
  while (child_iter->hasNext()) {
    Instance* child = child_iter->next();
    makeDbModule(child, module);
    if (not network_->isHierarchical(child)) {
      const char* child_name = network_->pathName(child);
      Cell* cell = network_->cell(child);
      dbMaster* master = getMaster(cell);
      if (master == nullptr) {
        logger_->warn(ORD,
                      1013,
                      "instance {} LEF master {} not found.",
                      child_name,
                      network_->name(cell));
        continue;
      }
      auto db_inst = dbInst::create(block_, master, child_name);
      if (db_inst == nullptr) {
        logger_->warn(ORD,
                      1015,
                      "leaf instance creation failed for {} of {}",
                      network_->name(child),
                      module->getName());
        continue;
      }
      module->addInst(db_inst);
    }
  }
  delete child_iter;

  //create dbModNets
  makeDbModuleNets(module, inst);

  if (module->getChildren().reversible() && module->getChildren().orderReversed())
    module->getChildren().reverse();
  if (module->getInsts().reversible() && module->getInsts().orderReversed())
    module->getInsts().reverse();
}

dbIoType
Verilog2db::staToDb(PortDirection* dir)
{
  if (dir == PortDirection::input())
    return dbIoType::INPUT;
  else if (dir == PortDirection::output())
    return dbIoType::OUTPUT;
  else if (dir == PortDirection::bidirect())
    return dbIoType::INOUT;
  else if (dir == PortDirection::tristate())
    return dbIoType::OUTPUT;
  else if (dir == PortDirection::unknown())
    return dbIoType::INPUT;
  else
    return dbIoType::INOUT;
}

void
Verilog2db::makeDbNets(const Instance* inst)
{
  bool is_top = (inst == network_->topInstance());
  NetIterator* net_iter = network_->netIterator(inst);
  while (net_iter->hasNext()) {
    Net* net = net_iter->next();
    const char* net_name = network_->pathName(net);
    if (is_top || !hasTerminals(net)) {
      dbNet* db_net = dbNet::create(block_, net_name);

      if (network_->isPower(net))
        db_net->setSigType(odb::dbSigType::POWER);
      if (network_->isGround(net))
        db_net->setSigType(odb::dbSigType::GROUND);

      // Sort connected pins for regression stability.
      PinSeq net_pins;
      NetConnectedPinIterator* pin_iter = network_->connectedPinIterator(net);
      while (pin_iter->hasNext()) {
        Pin* pin = pin_iter->next();
        net_pins.push_back(pin);
      }
      delete pin_iter;
      sort(net_pins, PinPathNameLess(network_));

      for (Pin* pin : net_pins) {
        if (network_->isTopLevelPort(pin)) {
          const char* port_name = network_->portName(pin);
          if (block_->findBTerm(port_name) == nullptr) {
            dbBTerm* bterm = dbBTerm::create(db_net, port_name);
            dbIoType io_type = staToDb(network_->direction(pin));
            bterm->setIoType(io_type);
          }
        }
        else if (network_->isLeaf(pin)) {
          const char* port_name = network_->portName(pin);
          Instance* inst = network_->instance(pin);
          const char* inst_name = network_->pathName(inst);
          dbInst* db_inst = block_->findInst(inst_name);
          if (db_inst) {
            dbMaster* master = db_inst->getMaster();
            dbMTerm* mterm = master->findMTerm(block_, port_name);
            if (mterm)
              db_inst->getITerm(mterm)->connect(db_net);
          }
        }
      }
    }
  }
  delete net_iter;

  InstanceChildIterator* child_iter = network_->childIterator(inst);
  while (child_iter->hasNext()) {
    const Instance* child = child_iter->next();
    makeDbNets(child);
  }
  delete child_iter;
}

bool
Verilog2db::hasTerminals(Net* net) const
{
  NetTermIterator* term_iter = network_->termIterator(net);
  bool has_terms = term_iter->hasNext();
  delete term_iter;
  return has_terms;
}

dbMaster*
Verilog2db::getMaster(Cell* cell)
{
  auto miter = master_map_.find(cell);
  if (miter != master_map_.end())
    return miter->second;
  else {
    const char* cell_name = network_->name(cell);
    dbMaster* master = db_->findMaster(cell_name);
    if (master) {
      master_map_[cell] = master;
      // Check for corresponding liberty cell.
      LibertyCell* lib_cell = network_->libertyCell(cell);
      if (lib_cell == nullptr)
        logger_->warn(
            ORD,
            1011,
            "LEF master {} has no liberty cell.",
            cell_name);
      return master;
    }
    else {
      LibertyCell* lib_cell = network_->libertyCell(cell);
      if (lib_cell)
        logger_->warn(
            ORD,
            1012,
            "Liberty cell {} has no LEF master.",
            cell_name);
      // OpenSTA read_verilog warns about missing cells.
      master_map_[cell] = nullptr;
      return nullptr;
    }
  }
}

}  // namespace ord
