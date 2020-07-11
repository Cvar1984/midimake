#include "include/MIDIfile.hpp"
#include <cstdio> // for std::fopen
#include <cstring>

int main(int argc, char **argv)
{
    // Now that we have a class that can create MIDI files, let's create
    // music.

    // Begin with some chords.
    static const int chords[][3] =
    {
        { 12,4,7 }, // +C  E  G  = 0
        { 12,9,5 }, // +C  A  F  = 1
        { 12,8,3 }, // +C  G# D# = 2
        { 12,7,3 }, // +C  G  D# = 3
        { 12,5,8 }, // +C  F  G# = 4
        { 12,3,8 }, // +C  D# G# = 5
        { 11,2,7 }, //  B  D  G  = 6
        { 10,2,7 }, // A#  D  G  = 7
        { 14,7,5 }, // +D  G  F  = 8
        { 14,7,11 },// +D  G  B  = 9
        { 14,19,11 }// +D +G  B  = 10
    };
    const char x = 99; // Arbitrary value we use here to indicate "no note"
    static const char chordline[64] =
    {
        0,x,0,0,x,0,x, 1,x,1,x,1,1,x,1,x,  2,x,2,2,x,2,x, 3,x,3,x,3,3,x,3,x,
        4,x,4,4,x,4,x, 5,x,5,x,5,5,x,5,x,  6,7,6,x,8,x,9,x,10,x,x,x,x,x,x,x
    };
    static const char chordline2[64] =
    {
        0,x,x,x,x,x,x, 1,x,x,x,x,x,x,x,x,  2,x,x,x,x,x,x, 3,x,x,x,x,x,x,x,x,
        4,x,x,x,x,x,x, 5,x,x,x,x,x,x,x,x,  6,x,x,x,x,x,x,x, 6,x,x,x,x,x,x,x
    };
    static const char bassline[64] =
    {
        0,x,x,x,x,x,x, 5,x,x,x,x,x,x,x,x,  8,x,x,0,x,3,x, 7,x,x,x,x,x,x,x,x,
        5,x,x,x,x,x,x, 3,x,x,x,x,x,x,x,x,  2,x,x,x,x,x,x,(char)-5,x,x,x,x,x,x,x,x
    };
    static const char fluteline[64] =
    {
        12,x,12,12, x,9, x, 17,x,16,x,14,x,12,x,x,
        8,x, x,15,14,x,12,  x,7, x,x, x,x, x,x,x,
        8,x, x, 8,12,x, 8,  x,7, x,8, x,3, x,x,x,
        5,x, 7, x, 2,x,(char)-5,  x,5, x,x, x,x, x,x,x
    };

    MIDIfile file;
    file.AddLoopStart();

    /* Choose instruments ("patches") for each channel: */
    static const char patches[16] =
    {
        0,0,0, 52,52,52, 48,48,48, 0,0,0,0,0, 35,74
            /* 0=piano, 52=choir aahs, 48=strings, 35=fretless bass, 74=pan flute */
    };
    for(unsigned c=0; c<16; ++c)
        if(c != 10) // Patch any other channel but not the percussion channel.
            file[0].Patch(c, patches[c]);

    int keys_on[16] = {-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1 };
    for(unsigned loops=0; loops<2; ++loops)
    {
        for(unsigned row=0; row<128; ++row)
        {
            for(unsigned c=0; c<16; ++c)
            {
                int note = x, add = 0, vol = 127;
                if(c < 3) // Piano chord
                { int chord = chordline[row%64];
                    if(chord != x) note = chords[chord][c%3], add=12*5, vol=0x4B; }
                else if(c >= 3 && c < 5) // Aux chord (choir)
                { int chord = chordline2[row%64];
                    if(chord != x) note = chords[chord][c%3], add=12*4, vol=0x50; }
                else if(c >= 6 && c < 8) // Aux chord (strings)
                { int chord = chordline2[row%64];
                    if(chord != x) note = chords[chord][c%3], add=12*5, vol=0x45; }
                else if(c == 14) // Bass
                    note = bassline[row%64], add=12*3, vol=0x6F;
                else if(c == 15 && row >= 64) // Flute
                    note = fluteline[row%64], add=12*5, vol=0x6F;
                if(note == x && (c<15 || row%31)) continue;
                file[0].KeyOff(c, keys_on[c], 0x20);
                keys_on[c] = -1;
                if(note == x) continue;
                file[0].KeyOn(c, keys_on[c] = note+add, vol);
            }
            file[0].AddDelay(160);
        }
        if(loops == 0) file.AddLoopEnd();
    }

    file.Finish();

    FILE *fp = std::fopen("test.mid", "wb");
    std::fwrite(&file.at(0), 1, file.size(), fp);
    std::fclose(fp);

    return 0;
}
