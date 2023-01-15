#define BOOST_TEST_MODULE TestModuleHierarchy
#include <boost/test/included/unit_test.hpp>
#include <iostream>
#include <string>

#include "db.h"
#include "helper.cpp"

using namespace odb;
using namespace std;
BOOST_AUTO_TEST_SUITE(test_suite)

struct F_DEFAULT
{
  F_DEFAULT()
  {
    db = createSimpleDB();
    block = db->getChip()->getBlock();
    lib = db->findLib("lib1");
  }
  ~F_DEFAULT() { dbDatabase::destroy(db); }
  dbDatabase* db;
  dbLib* lib;
  dbBlock* block;
};
BOOST_FIXTURE_TEST_CASE(test_default, F_DEFAULT)
{
  // dbModule::create() Succeed
  BOOST_TEST(dbModule::create(block, "parent_mod") != nullptr);
  dbModule* master_mod1 = dbModule::create(block, "master_mod1");
  dbModule* master_mod2 = dbModule::create(block, "master_mod2");
  // dbModule::create() rejected
  BOOST_TEST(dbModule::create(block, "parent_mod") == nullptr);
  // dbBlock::findModule()
  dbModule* parent_mod = block->findModule("parent_mod");
  BOOST_TEST(parent_mod != nullptr);
  // dbModule::getName()
  BOOST_TEST(string(parent_mod->getName()) == "parent_mod");
  // dbModInst::create() Succeed

  BOOST_TEST(master_mod1->getTerms().empty());
  BOOST_TEST(master_mod1->getNets().empty());
  BOOST_TEST(master_mod1->getInsts().empty());
  BOOST_TEST(master_mod2->getTerms().empty());
  BOOST_TEST(master_mod2->getNets().empty());
  BOOST_TEST(master_mod2->getInsts().empty());

  dbModTerm* i0_term = dbModTerm::create(master_mod1, "i0", dbIoType::INPUT);
  BOOST_TEST(i0_term != nullptr);
  BOOST_TEST(string(i0_term->getName()) == "i0");
  BOOST_TEST(i0_term->getIoType() == dbIoType::INPUT);
  BOOST_TEST(i0_term->getParent() == master_mod1);
  BOOST_TEST(i0_term->getNet() == nullptr);

  BOOST_TEST(!master_mod1->getTerms().empty());
  BOOST_TEST(master_mod1->getTerms().size() == 1);
  BOOST_TEST(master_mod1->findModTerm("i0") == i0_term);
  BOOST_TEST(master_mod1->findModTerm("io1") == nullptr);

  dbModTerm* io1_term = dbModTerm::create(master_mod1, "io1", dbIoType::INOUT);
  BOOST_TEST(io1_term != nullptr);
  BOOST_TEST(string(io1_term->getName()) == "io1");
  BOOST_TEST(io1_term->getIoType() == dbIoType::INOUT);
  BOOST_TEST(io1_term->getParent() == master_mod1);
  BOOST_TEST(io1_term->getNet() == nullptr);
  BOOST_TEST(master_mod1->getTerms().size() == 2);
  BOOST_TEST(master_mod1->findModTerm("io1") == io1_term);

  dbModTerm* o_term = dbModTerm::create(master_mod1, "o", dbIoType::OUTPUT);
  BOOST_TEST(o_term != nullptr);
  BOOST_TEST(string(o_term->getName()) == "o");
  BOOST_TEST(o_term->getIoType() == dbIoType::OUTPUT);
  BOOST_TEST(o_term->getParent() == master_mod1);
  BOOST_TEST(o_term->getNet() == nullptr);
  BOOST_TEST(master_mod1->getTerms().size() == 3);
  BOOST_TEST(master_mod1->findModTerm("o") == o_term);

  dbModTerm* mod2_i_term = dbModTerm::create(master_mod2, "i", dbIoType::INPUT);
  BOOST_TEST(mod2_i_term != nullptr);
  BOOST_TEST(string(mod2_i_term->getName()) == "i");
  BOOST_TEST(mod2_i_term->getIoType() == dbIoType::INPUT);
  BOOST_TEST(mod2_i_term->getParent() == master_mod2);
  BOOST_TEST(mod2_i_term->getNet() == nullptr);
  BOOST_TEST(master_mod2->getTerms().size() == 1);
  BOOST_TEST(master_mod2->findModTerm("i") == mod2_i_term);

  dbModNet* i0_net = dbModNet::create(master_mod1, "i0");
  BOOST_TEST(i0_net != nullptr);
  BOOST_TEST(string(i0_net->getName()) == "i0");
  BOOST_TEST(i0_net->getParent() == master_mod1);
  BOOST_TEST(i0_net->getTerms().empty());
  BOOST_TEST(i0_net->getTerms().size() == 0);
  BOOST_TEST(master_mod1->getNets().size() == 1);
  BOOST_TEST(master_mod1->findModNet("i0") == i0_net);
  BOOST_TEST(master_mod1->findModNet("i1") == nullptr);

  i0_term->connect(i0_net);
  BOOST_TEST(i0_term->getNet() == i0_net);
  BOOST_TEST(!i0_net->getTerms().empty());
  BOOST_TEST(i0_net->getTerms().size() == 1);

  io1_term->connect(i0_net);
  BOOST_TEST(io1_term->getNet() == i0_net);
  BOOST_TEST(i0_net->getTerms().size() == 2);

  // Freeze module before instantiation
  master_mod1->setFrozen();
  master_mod2->setFrozen();

  BOOST_TEST(dbModInst::create(parent_mod, master_mod1, "i1") != nullptr);
  // dbModInst::create() rejected duplicate name
  BOOST_TEST(dbModInst::create(parent_mod, master_mod1, "i1") == nullptr);
  // dbModInst::create() rejected master already has a modinst
  BOOST_TEST(dbModInst::create(parent_mod, master_mod1, "i2") == nullptr);
  // dbModule::findModInst()1
  dbModInst* mod1Inst = parent_mod->findModInst("i1");
  // dbModule getModInst()
  BOOST_TEST(master_mod1->getModInst() == mod1Inst);
  // dbModInst::getName()
  BOOST_TEST(string(mod1Inst->getName()) == "i1");
  // dbModule::getChildren()
  BOOST_TEST(parent_mod->getChildren().size() == 1);
  // dbBlock::getModInsts()
  BOOST_TEST(block->getModInsts().size() == 1);

  BOOST_TEST(mod1Inst->getITerms().size() == 3);
  dbModITerm* i0_iterm = mod1Inst->findITerm("i0");
  BOOST_TEST(i0_iterm != nullptr);
  BOOST_TEST(i0_iterm->getTerm() == i0_term);

  dbModITerm* io1_iterm = mod1Inst->findITerm("io1");
  BOOST_TEST(io1_iterm != nullptr);
  BOOST_TEST(io1_iterm->getTerm() == io1_term);

  dbModITerm* o_iterm = mod1Inst->findITerm("o");
  BOOST_TEST(o_iterm != nullptr);
  BOOST_TEST(o_iterm->getTerm() == o_term);

  BOOST_TEST(dbModInst::create(parent_mod, master_mod2, "i2") != nullptr);
  dbModInst* mod2Inst = parent_mod->findModInst("i2");
  BOOST_TEST(mod2Inst->getITerms().size() == 1);
  BOOST_TEST(master_mod2->getModInst() == mod2Inst);
  dbModITerm* i2_i_iterm = mod2Inst->findITerm("i");
  BOOST_TEST(i2_i_iterm != nullptr);
  BOOST_TEST(i2_i_iterm->getTerm() == mod2_i_term);

  dbModNet* n0_net = dbModNet::create(parent_mod, "n0");
  BOOST_TEST(n0_net != nullptr);
  BOOST_TEST(n0_net->getParent() == parent_mod);
  BOOST_TEST(n0_net->getName() == "n0");

  o_iterm->connect(n0_net);
  BOOST_TEST(o_iterm->getNet() == n0_net);
  i2_i_iterm->connect(n0_net);


  // dbInst <--> dbModule
  auto inst1 = dbInst::create(block, lib->findMaster("and2"), "inst1");
  auto inst2 = dbInst::create(block, lib->findMaster("and2"), "inst2");
  // dbModule::addInst()
  parent_mod->addInst(inst1);
  parent_mod->addInst(inst2);
  // dbModule::getInsts()
  BOOST_TEST(parent_mod->getInsts().size() == 2);
  // dbInst::getModule()
  BOOST_TEST(std::string(inst1->getModule()->getName()) == "parent_mod");
  // dbModule::addInst() new parent
  block->getTopModule()->addInst(inst2);
  BOOST_TEST(parent_mod->getInsts().size() == 1);
  BOOST_TEST(inst2->getModule() == block->getTopModule());
  // dbInst::destroy -> dbModule insts
  dbInst::destroy(inst1);
  BOOST_TEST(parent_mod->getInsts().size() == 0);
}
BOOST_FIXTURE_TEST_CASE(test_find_modinst, F_DEFAULT)
{
  auto top = block->getTopModule();
  BOOST_TEST(top != nullptr);
  auto master1 = odb::dbModule::create(block, "master1");
  odb::dbModInst::create(top, master1, "minst1");
  auto master2 = odb::dbModule::create(block, "master2");
  auto minst2 = odb::dbModInst::create(master1, master2, "minst2");
  BOOST_TEST(block->findModInst("minst1/minst2") == minst2);
}
struct F_DETAILED
{
  F_DETAILED()
  {
    db = createSimpleDB();
    block = db->getChip()->getBlock();
    lib = db->findLib("lib1");
    master_mod1 = dbModule::create(block, "master_mod1");
    master_mod2 = dbModule::create(block, "master_mod2");
    master_mod3 = dbModule::create(block, "master_mod3");
    parent_mod = dbModule::create(block, "parent_mod");
    i1 = dbModInst::create(parent_mod, master_mod1, "i1");
    i2 = dbModInst::create(parent_mod, master_mod2, "i2");
    i3 = dbModInst::create(parent_mod, master_mod3, "i3");
    inst1 = dbInst::create(block, lib->findMaster("and2"), "inst1");
    inst2 = dbInst::create(block, lib->findMaster("and2"), "inst2");
    inst3 = dbInst::create(block, lib->findMaster("and2"), "inst3");
    parent_mod->addInst(inst1);
    parent_mod->addInst(inst2);
    parent_mod->addInst(inst3);
  }
  ~F_DETAILED() { dbDatabase::destroy(db); }
  dbDatabase* db;
  dbLib* lib;
  dbBlock* block;
  dbModule* master_mod1;
  dbModule* master_mod2;
  dbModule* master_mod3;
  dbModule* parent_mod;
  dbModInst* i1;
  dbModInst* i2;
  dbModInst* i3;
  dbInst* inst1;
  dbInst* inst2;
  dbInst* inst3;
};
BOOST_FIXTURE_TEST_CASE(test_destroy, F_DETAILED)
{
  BOOST_TEST(block->getModInsts().size() == 3);
  // dbModInst::destroy()
  dbModInst::destroy(i1);
  BOOST_TEST(parent_mod->findModInst("i1") == nullptr);
  BOOST_TEST(block->getModInsts().size() == 2);
  // dbModule::destroy() master
  dbModule::destroy(master_mod2);
  BOOST_TEST(parent_mod->findModInst("i2") == nullptr);
  BOOST_TEST(block->getModInsts().size() == 1);
  // dbModule::destroy() parent
  dbModule::destroy(parent_mod);
  BOOST_TEST(block->findModule("parent_mod") == nullptr);
  BOOST_TEST(block->getModInsts().size() == 0);
}
BOOST_FIXTURE_TEST_CASE(test_iterators, F_DETAILED)
{
  int i;
  dbSet<dbModInst> children = parent_mod->getChildren();
  dbSet<dbModInst>::iterator modinst_itr;
  BOOST_TEST(children.size() == 3);
  BOOST_TEST(children.reversible());
  for (int j = 0; j < 2; j++) {
    if (j == 1)
      children.reverse();
    for (modinst_itr = children.begin(), i = j ? 1 : 3;
         modinst_itr != children.end();
         ++modinst_itr, i = i + (j ? 1 : -1))
      switch (i) {
        case 1:
          BOOST_TEST(*modinst_itr == i1);
          break;
        case 2:
          BOOST_TEST(*modinst_itr == i2);
          break;
        case 3:
          BOOST_TEST(*modinst_itr == i3);
          break;
        default:
          BOOST_TEST(false);
          break;
      }
  }

  dbSet<dbInst> insts = parent_mod->getInsts();
  dbSet<dbInst>::iterator inst_itr;
  BOOST_TEST(insts.size() == 3);
  BOOST_TEST(insts.reversible());
  for (int j = 0; j < 2; j++) {
    if (j == 1)
      insts.reverse();
    for (inst_itr = insts.begin(), i = j ? 1 : 3; inst_itr != insts.end();
         ++inst_itr, i = i + (j ? 1 : -1))
      switch (i) {
        case 1:
          BOOST_TEST(*inst_itr == inst1);
          break;
        case 2:
          BOOST_TEST(*inst_itr == inst2);
          break;
        case 3:
          BOOST_TEST(*inst_itr == inst3);
          break;
        default:
          BOOST_TEST(false);
          break;
      }
  }
}

BOOST_AUTO_TEST_SUITE_END()
