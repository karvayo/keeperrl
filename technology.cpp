/* Copyright (C) 2013-2014 Michal Brzozowski (rusolis@poczta.fm)

   This file is part of KeeperRL.

   KeeperRL is free software; you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   KeeperRL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.
   If not, see http://www.gnu.org/licenses/ . */

#include "stdafx.h"
#include "technology.h"
#include "util.h"
#include "skill.h"

template <class Archive> 
void Technology::serialize(Archive& ar, const unsigned int version) {
  ar& SUBCLASS(Singleton)
    & SVAR(name)
    & SVAR(cost)
    & SVAR(prerequisites)
    & SVAR(research);
  CHECK_SERIAL;
}

SERIALIZABLE(Technology);

SERIALIZATION_CONSTRUCTOR_IMPL(Technology);

void Technology::init() {
  Technology::set(TechId::ALCHEMY, new Technology("alchemy", 80, {}));
  Technology::set(TechId::ALCHEMY_ADV, new Technology("advanced alchemy", 200, {TechId::ALCHEMY}));
  Technology::set(TechId::HUMANOID_MUT, new Technology("humanoid mutation", 400,{}));
  Technology::set(TechId::BEAST_MUT, new Technology("beast mutation", 400, {}));
  Technology::set(TechId::CRAFTING, new Technology("crafting", 40, {}));
  Technology::set(TechId::IRON_WORKING, new Technology("iron working", 60, {TechId::CRAFTING}));
  Technology::set(TechId::JEWELLERY, new Technology("jewellery", 200, {TechId::IRON_WORKING}));
  Technology::set(TechId::TWO_H_WEAP, new Technology("two-handed weapons", 100, {TechId::IRON_WORKING}, true));
  Technology::set(TechId::TRAPS, new Technology("traps", 100, {TechId::CRAFTING}));
  Technology::set(TechId::ARCHERY, new Technology("archery", 100, {TechId::CRAFTING}, true));
  Technology::set(TechId::SPELLS, new Technology("sorcery", 60, {}));
  Technology::set(TechId::SPELLS_ADV, new Technology("advanced sorcery", 120, {TechId::SPELLS}));
  Technology::set(TechId::SPELLS_MAS, new Technology("master sorcery", 350, {TechId::SPELLS_ADV}));
  Technology::set(TechId::KRAKEN, new Technology("kraken breeding", 150, {TechId::ALCHEMY}));
}

bool Technology::canResearch() const {
  return research;
}

int Technology::getCost() const {
  return cost;
}

vector<Technology*> Technology::getNextTechs(const vector<Technology*>& current) {
  vector<Technology*> ret;
  for (Technology* t : Technology::getAll())
    if (t->canLearnFrom(current) && !contains(current, t))
      ret.push_back(t);
  return ret;
}

Technology::Technology(const string& n, int c, const vector<TechId>& pre, bool canR)
    : name(n), cost(c), research(canR) {
  for (TechId id : pre)
    prerequisites.push_back(Technology::get(id));
}

bool Technology::canLearnFrom(const vector<Technology*>& techs) const {
  vector<Technology*> myPre = prerequisites;
  for (Technology* t : techs)
    removeElementMaybe(myPre, t);
  return myPre.empty();
}

const string& Technology::getName() const {
  return name;
}

vector<Technology*> Technology::getSorted() {
  vector<Technology*> ret;
  while (ret.size() < getAll().size()) {
    append(ret, getNextTechs(ret));
  }
  return ret;
}

const vector<Technology*> Technology::getPrerequisites() const {
  return prerequisites;
}
  
const vector<Technology*> Technology::getAllowed() const {
  vector<Technology*> ret;
  for (Technology* t : getAll())
    if (contains(t->prerequisites, this))
      ret.push_back(t);
  return ret;
}
