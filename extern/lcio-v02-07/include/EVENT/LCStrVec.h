// -*- C++ -*-
// AID-GENERATED
// =========================================================================
// This class was generated by AID - Abstract Interface Definition          
// DO NOT MODIFY, but use the org.freehep.aid.Aid utility to regenerate it. 
// =========================================================================
#ifndef EVENT_LCSTRVEC_H
#define EVENT_LCSTRVEC_H 1

#include "EVENT/LCObject.h"
#include "LCIOSTLTypes.h"

 #include "IMPL/AccessChecked.h" // FIXME: EVENT shouldn't  depend on IMPL ...

namespace EVENT {

/** String vector used for user extensions.
 * The cpp implementation is a std::vector<string>
 * 
 * @author vogt
 * @version Jun 18, 2004
 * @see LCObject
 */
class LCStrVec : public LCObject, public StringVec {

public: 
    /// Destructor.
    virtual ~LCStrVec() { /* nop */; }


    typedef LCStrVec lcobject_type ;

    public:
    int id() const { return _acc.simpleUID() ; } 
    protected:
      IMPL::AccessChecked _acc ;
}; // class
} // namespace EVENT
#endif /* ifndef EVENT_LCSTRVEC_H */
