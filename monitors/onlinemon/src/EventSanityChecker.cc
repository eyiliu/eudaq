/*
 * EventSanityChecker.cc
 *
 *  Created on: Jun 16, 2011
 *      Author: stanitz
 */

#include "EventSanityChecker.hh"

#include <iostream>

// default constructor
EventSanityChecker::EventSanityChecker() { NPlanes = 0; }

// set the number of planes and frames
EventSanityChecker::EventSanityChecker(int nplanes) { NPlanes = nplanes; }

EventSanityChecker::~EventSanityChecker() {
  // TODO Auto-generated destructor stub
}

unsigned int EventSanityChecker::getNPlanes() const { return NPlanes; }

void EventSanityChecker::setNPlanes(int NPlanes, std::vector<std::string> ids) {
  this->NPlanes = NPlanes;
  m_ids = ids;
}

void EventSanityChecker::reportIds() const {
  std::cout << "Initial IDs\n";
  
  for(auto &s: m_ids) {
    std::cout << " " << s << '\n';
  }
}
