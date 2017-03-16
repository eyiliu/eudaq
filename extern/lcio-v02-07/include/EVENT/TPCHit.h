// -*- C++ -*-
// AID-GENERATED
// =========================================================================
// This class was generated by AID - Abstract Interface Definition          
// DO NOT MODIFY, but use the org.freehep.aid.Aid utility to regenerate it. 
// =========================================================================
#ifndef EVENT_TPCHIT_H
#define EVENT_TPCHIT_H 1

#include "EVENT/LCObject.h"

namespace EVENT {

/** Deprecated. Please use TrackerRawData, TrackerData and TrackerPulse instead.
 *  A future version of LCIO will not support writing of TPCHit collections anymore.
 * 
 * @deprecated
 * @author gaede
 * @version $Id: TPCHit.aid,v 1.5 2006-03-24 13:25:52 gaede Exp $
 */

class TPCHit : public LCObject {

public: 
    /// Destructor.
    virtual ~TPCHit() { /* nop */; }


    /** Useful typedef for template programming with LCIO */
    typedef TPCHit lcobject_type ;

    /** Returns the detector specific cell id.
     */
    virtual int getCellID() const = 0;

    /** Returns the  time of the hit.
     */
    virtual float getTime() const = 0;

    /** Returns the integrated charge of the hit.
     */
    virtual float getCharge() const = 0;

    /** Returns a quality flag for the hit.
     */
    virtual int getQuality() const = 0;

    /** Return the number of raw data (32-bit) words stored for the hit.
     *  Check the flag word (bit TPCBIT_RAW) of the collection if raw data is
     *  stored at all.
     */
    virtual int getNRawDataWords() const = 0;

    /** Return the raw data (32-bit) word at i.
     *  Check the flag word (bit TPCBIT_RAW) of the collection if raw data is
     *  stored at all.
     */
    virtual int getRawDataWord(int i) const = 0;
}; // class
} // namespace EVENT
#endif /* ifndef EVENT_TPCHIT_H */
