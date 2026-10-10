#ifndef FLYINGDODGEM_SCENE_H
#define FLYINGDODGEM_SCENE_H

#include <stddef.h> /* size_t: the placement operator new below */
#include "flyingdodgem_types.h"

/* The scene object HOVER publishes into g_460970 (and which F407160 then
 * dereferences), constructed at HOVER 0x41e230.
 *
 * The layout is transcribed from the retail constructor: the base subobject is
 * built first, then the four member objects in declaration order, then the
 * vtable pointer, then the body.  Everything retail leaves untouched in this
 * constructor is a padding member so that the offsets below stay exact. */

class CBaseWorld
{
public:
    CBaseWorld();
    virtual ~CBaseWorld();
};

/* Leaf objects constructed from the scene TU. */
class CWorldSide
{
public:
    CWorldSide();
    ~CWorldSide();
};

class CWorldSpan
{
public:
    CWorldSpan();
    ~CWorldSpan();
};

/* Fixed-size table.  CWorld allocates two of them at +0x838c and +0x83a8;
 * CThingDerived::CThingDerived only forwards to CThing, which puts the call
 * and the vtable store in the scene constructor exactly where retail has them. */
class CThing
{
public:
    CThing(int32_t nBlockSize);
    virtual ~CThing();
};

class CThingDerived : public CThing
{
public:
    CThingDerived(int32_t nBlockSize) : CThing(nBlockSize) {}
    virtual ~CThingDerived();

private:
    uint8_t m_reserved[0x18];
};

class CWorld : public CBaseWorld
{
public:
    CWorld();

    virtual void Update();

private:
    /* 0x0004 */ uint8_t      m_pad0[0x5c];
    /* 0x0060 */ CWorldSide   m_side;
    /* 0x0061 */ uint8_t      m_pad1[0x123];
    /* 0x0184 */ int32_t      m_nTick;
    /* 0x0188 */ int32_t      m_nFrame;
    /* 0x018c */ uint8_t      m_pad2[0x8000];
    /* 0x818c */ CWorldSpan   m_span;
    /* 0x818d */ uint8_t      m_pad3[0x1fb];
    /* 0x8388 */ int32_t      m_nFlags;
    /* 0x838c */ CThingDerived m_thing1;
    /* 0x83a8 */ CThingDerived m_thing2;
    /* 0x83c4 */ uint8_t      m_pad4[8];
    /* 0x83cc */ int32_t      m_nColumns;
    /* 0x83d0 */ uint8_t      m_pad5[0x14];
    /* 0x83e4 */ int32_t      m_nRows;
    /* 0x83e8 */ uint8_t      m_pad6[4];
    /* 0x83ec */ uint32_t     m_grid[34];
    /* 0x8474 */ int32_t      m_nReserved;
    /* 0x8478 */ double       m_dInertia;
    /* 0x8480 */ double       m_dAccel1;
    /* 0x8488 */ double       m_dAccel2;
    /* 0x8490 */ double       m_dAccel3;
    /* 0x8498 */ int32_t      m_nActive;
};

/* Beacons planted into a level by the scene build (HOVER 0x423ea0).  The
 * build walks the map at g_4a4768 twice: once to count the "BEACON_" entries
 * so the beacon table can be sized, then again to take one CBeacon per entry
 * off the free list in g_4c57a8. */

class CBeaconKey
{
public:
    CBeaconKey();
    ~CBeaconKey();
    int32_t M44bdef(uint32_t nID);

    /* 0x0000 */ char*   m_psz;
    /* 0x0004 */ int32_t m_nPad1;
    /* 0x0008 */ int32_t m_nPad2;
};

/* Value half of the map pair the build reads; only the tail is touched. */
class CBeaconValue
{
public:
    /* 0x0000 */ uint8_t m_pad0[0x14];
    /* 0x0014 */ int16_t m_nId;
    /* 0x0016 */ int16_t m_nType;
};

class CBeaconBase
{
public:
    CBeaconBase();
    virtual ~CBeaconBase();

    /* 0x0004 */ uint8_t m_pad0[0x40];
    /* 0x0044 */ int16_t m_nFlags1;
    /* 0x0046 */ int16_t m_nId1;
    /* 0x0048 */ int16_t m_nFlags2;
    /* 0x004a */ int16_t m_nId2;
    /* 0x004c */ uint8_t m_pad1[0x38];
};

class CBeaconSubBase
{
public:
    CBeaconSubBase();
    virtual ~CBeaconSubBase();

    /* 0x0004 */ uint8_t m_pad0[0x10];
};

class CBeaconSub : public CBeaconSubBase
{
public:
    CBeaconSub() {}
    virtual ~CBeaconSub();
};

class CBeacon : public CBeaconBase
{
public:
    CBeacon(CBeaconValue* value)
    {
        m_nId1    = value->m_nId;
        m_pValue  = value;
        m_nFlag   = 0;
        m_nFlags1 = 0;
        m_nFlags2 = 0;
        m_nId2    = value->m_nType;
    }
    virtual ~CBeacon();

    /* 0x0084 */ CBeaconSub    m_sub;
    /* 0x0098 */ CBeaconValue* m_pValue;
    /* 0x009c */ uint8_t       m_pad0[0xc];
    /* 0x00a8 */ int32_t       m_nFlag;
    /* 0x00ac */ CBeacon*      m_pNext;
};

/* Placement form of operator new: the scene build hands out CBeacon storage
 * from its own free list and only runs the constructor over it. */
inline void* operator new(size_t, void* p) { return p; }

#endif /* FLYINGDODGEM_SCENE_H */
