#ifndef FLYINGDODGEM_SINE_NATIVE_H
#define FLYINGDODGEM_SINE_NATIVE_H

/* Fill the shared fixed-point sine table (retail BSS 0x4aca58, 32768 bytes).
 * Retail ships the table uninitialized and generates it at startup (it must:
 * the file carries no bytes for it); the matching build therefore keeps the
 * BSS zeros, exactly as shipped. The native build needs real values: without
 * them every sine/cosine read is 0, the F4055b0 minimap/world rotation
 * collapses to identity and all heading-relative blips pile up unrotated.
 * Safe to call more than once. Native builds only; the matching build never
 * compiles this unit. */
void SineNative_Init(void);

#endif
