#pragma once

#if defined(PLATFORM_SIM)

#include "model/ArpSequence.h"
#include "model/NoteSequence.h"
#include "model/Scale.h"

namespace EngineTestHooks {

float evalNoteStepNoteForScale(const NoteSequence::Step &step, int probabilityBias, const Scale &scale, int rootNote, int octave, int transpose, bool useVariation = false);
float evalArpStepNoteForScale(const ArpSequence::Step &step, int probabilityBias, const Scale &scale, int rootNote, int octave, int transpose, const ArpSequence &sequence, bool useVariation = false, bool forceScaleTransposition = false);
bool arpPitchSlotActiveForScale(const ArpSequence::Step &step, int slotIndex, const Scale &scale, bool forceScaleTransposition = false);

} // namespace EngineTestHooks

#endif // defined(PLATFORM_SIM)
