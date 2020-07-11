#ifndef MIDITRACK_HPP
#define MIDITRACK_HPP
#include <cstring> // for std::strlen
#include "MIDIvec.hpp"

/* Define a class which encodes MIDI events into a track */
class MIDItrack: public MIDIvec
{
    protected:
        unsigned delay, running_status;
    public:
        MIDItrack()
            : MIDIvec(), delay(0), running_status(0)
        {
        }

        // Methods for indicating how much time elapses:
        void AddDelay(unsigned amount) { delay += amount; }

        void AddVarLen(unsigned t)
        {
            if(t >> 21) AddBytes(0x80 | ((t >> 21) & 0x7F));
            if(t >> 14) AddBytes(0x80 | ((t >> 14) & 0x7F));
            if(t >>  7) AddBytes(0x80 | ((t >>  7) & 0x7F));
            AddBytes(((t >> 0) & 0x7F));
        }

        void Flush()
        {
            AddVarLen(delay);
            delay = 0;
        }

        // Methods for appending events into the track:
        template<typename... Args>
            void AddEvent(byte data, Args...args)
            {
                /* MIDI tracks have the following structure:
                 *
                 * { timestamp [metaevent ... ] event } ...
                 *
                 * Each event is prefixed with a timestamp,
                 * which is encoded in a variable-length format.
                 * The timestamp describes the amount of time that
                 * must be elapsed before this event can be handled.
                 *
                 * After the timestamp, comes the event data.
                 * The first byte of the event always has the high bit on,
                 * and the remaining bytes always have the high bit off.
                 *
                 * The first byte can however be omitted; in that case,
                 * it is assumed that the first byte is the same as in
                 * the previous command. This is called "running status".
                 * The event may furthermore beprefixed
                 * with a number of meta events.
                 */
                Flush();
                if(data != running_status) AddBytes(running_status = data);
                AddBytes(args...);
            }
        void AddEvent() { }

        template<typename... Args>
            void AddMetaEvent(byte metatype, byte nbytes, Args...args)
            {
                Flush();
                AddBytes(0xFF, metatype, nbytes, args...);
            }

        // Key-related parameters: channel number, note number, pressure
        void KeyOn(int ch, int n, int p)    { if(n>=0)AddEvent(0x90|ch, n, p); }
        void KeyOff(int ch, int n, int p)   { if(n>=0)AddEvent(0x80|ch, n, p); }
        void KeyTouch(int ch, int n, int p) { if(n>=0)AddEvent(0xA0|ch, n, p); }
        // Events with other types of parameters:
        void Control(int ch, int c, int v) { AddEvent(0xB0|ch, c, v); }
        void Patch(int ch, int patchno)    { AddEvent(0xC0|ch, patchno); }
        void Wheel(int ch, unsigned value)
        { AddEvent(0xE0|ch, value&0x7F, (value>>7)&0x7F); }

        // Methods for appending metadata into the track:
        void AddText(int texttype, const char* text)
        {
            AddMetaEvent(texttype, std::strlen(text), text);
        }
};
#endif
