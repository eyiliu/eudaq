/*
 * EventSanityChecker.hh
 *
 *  Created on: Jun 16, 2011
 *      Author: stanitz
 */

#ifndef EVENTSANITY_H_
#define EVENTSANITY_H_

#include <string>
#include <vector>

class EventSanityChecker {
public:
  EventSanityChecker();
  EventSanityChecker(int nplanes);
  virtual ~EventSanityChecker();
  unsigned int getNPlanes() const;
  void setNPlanes(int NPlanes, std::vector<std::string> ids);
  void reportIds() const;

private:
  unsigned int NPlanes;
  std::vector<std::string> m_ids;
};

#endif /* EVENTSANITY_H_ */
