# Missing
A cllection of documented missing functions from Orca Emulator.

## Command Processor
 * No interrupts.

## XF
### Transformation Unit
 * Possible division by zero causing UIs to not render at the zfar plane.

### Normal Unit
 * Normal 0 unnormalized.

### TexCoordRegularUnit
 * Does not convert NaNs to 1. https://bugs.dolphin-emu.org/issues/11458
 * Special case when q is 0 where data is clamped.
 * Missing Normalize Function