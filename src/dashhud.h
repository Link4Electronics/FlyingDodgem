#ifndef FLYINGDODGEM_DASHHUD_H
#define FLYINGDODGEM_DASHHUD_H

#include "flyingdodgem_types.h"

/* Repaint the dashboard's seven segmented gauges onto the screen DC.
 * The GDI objects (pen + lit/unlit brushes) are created on first use and kept
 * for the whole session. Safe to call with a NULL DC. */
void __cdecl DashHud_Paint(void *screen);

#endif
