#ifndef MIDIFILE_HPP
#define MIDIFILE_HPP
#include <vector>		// For std::vector<>
#include "MIDIvec.hpp"
#include "MIDItrack.hpp"

/* Define a class that encapsulates all methods needed to craft a MIDI file. */
class MIDIfile: public MIDIvec
{
    protected:
        std::vector < MIDItrack > tracks;
        unsigned deltaticks, tempo;
    public:
        MIDIfile ():MIDIvec (), tracks (), deltaticks (1000), tempo (1000000)
    {
    }

        void AddLoopStart ()
        {
            (*this)[0].AddText (6, "loopStart");
        }
        void AddLoopEnd ()
        {
            (*this)[0].AddText (6, "loopEnd");
        }

        MIDItrack & operator[](unsigned trackno)
        {
            if (trackno >= tracks.size ()) {
                tracks.reserve (16); tracks.resize (trackno + 1);
            }

            MIDItrack & result = tracks[trackno]; if (result.empty ())
            {
                // Meta 0x58 (misc settings):
                //      time signature: 4/2
                //      ticks/metro:    24
                //      32nd per 1/4:   8
                result.AddMetaEvent (0x58, 4, 4, 2, 24, 8);
                // Meta 0x51 (tempo):
                result.AddMetaEvent (0x51, 3, tempo >> 16, tempo >> 8, tempo);
            }
            return result;
        }

        void Finish ()
        {
            clear (); AddBytes (
                    // MIDI signature (MThd and number 6)
                    "MThd", 0, 0, 0, 6,
                    // Format number (1: multiple tracks, synchronous)
                    0, 1,
                    tracks.size () >> 8, tracks.size (),
                    deltaticks >> 8, deltaticks);
            for (unsigned a = 0; a < tracks.size (); ++a)
            {
                // Add meta 0x2F to the track, indicating the track end:
                tracks[a].AddMetaEvent (0x2F, 0);
                // Add the track into the MIDI file:
                AddBytes ("MTrk",
                        tracks[a].size () >> 24,
                        tracks[a].size () >> 16,
                        tracks[a].size () >> 8,
                        tracks[a].size () >> 0);
                insert (end (), tracks[a].begin (), tracks[a].end ());
            }
        }
};
#endif
