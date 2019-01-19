//
// VowelHelper.cpp
//
// Copyright (c) 2006-2010 Lukhnos D. Liu (http://lukhnos.org)
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

#include "VowelHelper.h"
#include <algorithm>

using namespace Formosa::TaiwaneseRomanization;

static const unsigned int kPOJ9thTone = 10;
map<string, map<unsigned int, string> >* VowelHelper::c_vowelTable = 0;
map<string, string>* VowelHelper::c_toUppercasedVowelTable = 0;
VowelDecompositionState *VowelHelper::c_groundState = 0;

const string VowelHelper::symbolForVowel(const string& vowel, unsigned int tone, bool usePOJStyleOUAndNN, bool usePOJStyleNinthToneMark, bool composeII, bool usePOJLegacyOU)
{
    // This singleton instantiation is not thread safe, you've been warned
    if (!c_vowelTable) {
        populateVowelTables();
    }
    
    // if tone is out of range, return
    if (tone > 9 || !vowel.length()) {
        return "";
    }

    // if tone is 0, treat as tone 1; and subsc
    unsigned int realtone = tone;
    
    if (!realtone) {
        realtone = 1;
    }
    else if (realtone == 9 && usePOJStyleNinthToneMark) {
        realtone = kPOJ9thTone;
    }
    
    string lowercasedVowel = vowel;
    transform(lowercasedVowel.begin(), lowercasedVowel.end(), lowercasedVowel.begin(), ::tolower);
    

    if (lowercasedVowel == "q") {
        // we reject q, only take the real ou
        return "";
    }
    
    if (lowercasedVowel == "nn") {
        return usePOJStyleOUAndNN ? string("\xe2\x81\xbf") : vowel; // ⁿ
    }
    
    string coda;    // for processing oo/OO/Oo/oO only
    string lookupString = vowel;
    
    if (lowercasedVowel == "ou" || lowercasedVowel == "oo") {
        if (usePOJStyleOUAndNN) {
            if (usePOJLegacyOU) {
              lookupString = ((vowel[0] == 'O') ? "Q" : "q");
            } else {
              lookupString = ((vowel[0] == 'O') ? "Z" : "z");
            }
        }
        else {
            lookupString = string(1, vowel[0]);
            coda = string(1, vowel[1]);
        }
    }
    
    if (composeII && lowercasedVowel == "ii") {
        lookupString = ((vowel[0] == 'I') ? "II" : "ii");
    }
    
    map<string, map<unsigned int, string> >::const_iterator i = c_vowelTable->find(lookupString);
    if (i == c_vowelTable->end()) {
        return "";
    }
    
    const map<unsigned int, string>& toneVowelTable = (*i).second;
    map<unsigned int, string>::const_iterator j = toneVowelTable.find(realtone);
    if (j == toneVowelTable.end()) {
        return "";
    }
    
    return coda.length() ? ((*j).second + coda) : (*j).second;
}

char VowelHelper::diacriticShorthandFromTone(unsigned int tone)
{
    switch (tone) {
        case 2: return '\'';
        case 3: return '`';
        case 5: return '^';
        case 6: return '/';
        case 7: return '=';
        case 8: return '|';
        case 9: return '\"';
    }
    
    return 0;    
}

unsigned int VowelHelper::toneFromDiacriticShorthand(char shorthand)
{
    switch (shorthand) {
        case '\'': return 2;
        case '`' : return 3;
        case '^' : return 5;
        case '/' : return 6;
        case '=' : return 7;
        case '|' : return 8;
        case '\"': return 9;
    }
    
    return 0;    
}

bool VowelHelper::isDiacriticShorthand(char shorthand)
{
    return (shorthand == '\'' || shorthand == '`' || shorthand == '^' || shorthand == '/' || shorthand == '=' || shorthand == '|' || shorthand == '\"');
}

const string VowelHelper::uppercasedVowelFromVowel(const string& vowel)
{
    if (!c_toUppercasedVowelTable) {
        populateVowelTables();        
    }
    
    map<string, string>::const_iterator i = c_toUppercasedVowelTable->find(vowel);
    if (i == c_toUppercasedVowelTable->end()) {
        string uppercased = vowel;
        transform(uppercased.begin(), uppercased.end(), uppercased.begin(), ::toupper);        
        
        return uppercased;
    }

    return (*i).second;
}

const string VowelHelper::queryFormFromComposedForm(const string& composed)
{
    unsigned int finalTone = 0;
    const VowelDecompositionState* groundState = &vowelDecompositionGroundState();
    const VowelDecompositionState* state = groundState;

    string accu;
    string result;
    string::const_iterator i = composed.begin();
    
    while (i != composed.end()) {
        map<char, VowelDecompositionState>::const_iterator t = state->transitions.find(*i);
        if (t == state->transitions.end()) {
            if (state == groundState) {
                result += *i;
                ++i;
            }
            else if (state->representedSymbol.length()) {
                result += state->representedSymbol;
                finalTone = state->representedTone;
            }
            else {
                result += accu;
            }

            accu = "";
            state = groundState;
        }
        else {
            state = &((*t).second);
            accu += *i;
            ++i;
        }
    }

    // if we have unfinished emission
    if (state != groundState) {
        if (state->representedSymbol.length()) {
            result += state->representedSymbol;
            finalTone = state->representedTone;
        }
        else {
            result += accu;
        }
    }
        
    if (finalTone > 1) {
        result += ('0' + finalTone);
    }
    
    return result;
}


VowelDecompositionState& VowelHelper::vowelDecompositionGroundState()
{
    if (!c_groundState) {
        populateDecompositionStateMachine();
    }
    
    return *c_groundState;
}

void VowelHelper::populateVowelTables()
{
    if (!c_vowelTable) {
        c_vowelTable = new map<string, map<unsigned int, string> >;
        map<string, map<unsigned int, string> >& table = *c_vowelTable;
        
        table["a"][1] = "a";					// a
        table["a"][2] = "\xc3\xa1";				// á
        table["a"][3] = "\xc3\xa0";				// à
        table["a"][4] = "a";					// a
        table["a"][5] = "\xc3\xa2";				// â
        table["a"][6] = "\xc7\x8e";				// ǎ
        table["a"][7] = "\xc4\x81";				// ā
        table["a"][8] = "a\xcc\x8d";			// a̍
        table["a"][9] = "a\xcc\x8b";			// a̋
        table["a"][kPOJ9thTone] = "\xc4\x83";	// ă
        table["e"][1] = "e";					// e
        table["e"][2] = "\xc3\xa9";				// é
        table["e"][3] = "\xc3\xa8";				// è
        table["e"][4] = "e";					// e
        table["e"][5] = "\xc3\xaa";				// ê
        table["e"][6] = "\xc4\x9b";				// ě
        table["e"][7] = "\xc4\x93";				// ē
        table["e"][8] = "e\xcc\x8d";			// e̍
        table["e"][9] = "e\xcc\x8b";			// e̋
        table["e"][kPOJ9thTone] = "\xc4\x95";	// ĕ
        table["i"][1] = "i";					// i
        table["i"][2] = "\xc3\xad";				// í
        table["i"][3] = "\xc3\xac";				// ì
        table["i"][4] = "i";					// i
        table["i"][5] = "\xc3\xae";				// î
        table["i"][6] = "\xc7\x90";				// ǐ
        table["i"][7] = "\xc4\xab";				// ī
        table["i"][8] = "i\xcc\x8d";			// i̍
        table["i"][9] = "i\xcc\x8b";			// i̋
        table["i"][kPOJ9thTone] = "\xc4\xad";	// ĭ
        table["m"][1] = "m";					// m
        table["m"][2] = "\xe1\xb8\xbf";			// ḿ
        table["m"][3] = "m\xcc\x80";			// m̀
        table["m"][4] = "m";					// m
        table["m"][5] = "m\xcc\x82";			// m̂
        table["m"][6] = "m\xcc\x8c";			// m̌
        table["m"][7] = "m\xcc\x84";			// m̄
        table["m"][8] = "m\xcc\x8d";			// m̍
        table["m"][9] = "m\xcc\x8b";			// m̋
        table["m"][kPOJ9thTone] = "m\xcc\x86";	// m̆
        table["n"][1] = "n";					// n
        table["n"][2] = "\xc5\x84";				// ń
        table["n"][3] = "\xc7\xb9";				// ǹ
        table["n"][4] = "n";					// n
        table["n"][5] = "n\xcc\x82";			// n̂
        table["n"][6] = "n\xcc\x8c";			// ň
        table["n"][7] = "n\xcc\x84";			// n̄
        table["n"][8] = "n\xcc\x8d";			// n̍
        table["n"][9] = "n\xcc\x8b";			// n̋
        table["n"][kPOJ9thTone] = "n\xcc\x86";	// n̆
        table["o"][1] = "o";					// o
        table["o"][2] = "\xc3\xb3";				// ó
        table["o"][3] = "\xc3\xb2";				// ò
        table["o"][4] = "o";					// o
        table["o"][5] = "\xc3\xb4";				// ô
        table["o"][6] = "\xc7\x92";				// ǒ
        table["o"][7] = "\xc5\x8d";				// ō
        table["o"][8] = "o\xcc\x8d";			// o̍
        table["o"][9] = "\xc5\x91";				// ő
        table["o"][kPOJ9thTone] = "\xc5\x8f";	// ŏ
        table["q"][1] = "o\xcd\x98";			// o͘
        table["q"][2] = "\xc3\xb3\xcd\x98";		// ó͘
        table["q"][3] = "\xc3\xb2\xcd\x98";		// ò͘
        table["q"][4] = "o\xcd\x98";			// o͘
        table["q"][5] = "\xc3\xb4\xcd\x98";		// ô͘
        table["q"][6] = "\xc7\x92\xcd\x98";		// ǒ͘
        table["q"][7] = "\xc5\x8d\xcd\x98";		// ō͘
        table["q"][8] = "o\xcc\x8d\xcd\x98";	// o̍͘
        table["q"][9] = "o\xcc\x8b\xcd\x98";	// ő͘
        table["q"][kPOJ9thTone] = "\xc5\x8f\xcd\x98";	// ŏ͘
        table["z"][1] = "o\u0358";
        table["z"][2] = "o\u0358\u0301";
        table["z"][3] = "o\u0358\u0300";
        table["z"][4] = "o\u0358";
        table["z"][5] = "o\u0358\u0302";
        table["z"][6] = "o\u0358\u030c";
        table["z"][7] = "o\u0358\u0304";
        table["z"][8] = "o\u0358\u030d";
        table["z"][9] = "o\u0358\u030b";
        table["z"][kPOJ9thTone] = "o\u0358\u0306";
        table["u"][1] = "u";					// u
        table["u"][2] = "\xc3\xba";				// ú
        table["u"][3] = "\xc3\xb9";				// ù
        table["u"][4] = "u";					// u
        table["u"][5] = "\xc3\xbb";				// û
        table["u"][6] = "\xc7\x94";				// ǔ
        table["u"][7] = "\xc5\xab";				// ū
        table["u"][8] = "u\xcc\x8d";			// u̍
        table["u"][9] = "\xc5\xb1";				// ű
        table["u"][kPOJ9thTone] = "\xc5\xad";	// ŭ
        table["A"][1] = "A";					// A
        table["A"][2] = "\xc3\x81";				// Á
        table["A"][3] = "\xc3\x80";				// À
        table["A"][4] = "A";					// A
        table["A"][5] = "\xc3\x82";				// Â
        table["A"][6] = "\xc7\x8d";				// Ǎ
        table["A"][7] = "\xc4\x80";				// Ā
        table["A"][8] = "A\xcc\x8d";			// A̍
        table["A"][9] = "A\xcc\x8b";			// A̋
        table["A"][kPOJ9thTone] = "\xc4\x82";	// Ă
        table["E"][1] = "E";					// E
        table["E"][2] = "\xc3\x89";				// É
        table["E"][3] = "\xc3\x88";				// È
        table["E"][4] = "E";					// E
        table["E"][5] = "\xc3\x8a";				// Ê
        table["E"][6] = "\xc4\x9a";				// Ě
        table["E"][7] = "\xc4\x92";				// Ē
        table["E"][8] = "E\xcc\x8d";			// E̍
        table["E"][9] = "E\xcc\x8b";			// E̋
        table["E"][kPOJ9thTone] = "\xc4\x94";	// Ĕ
        table["I"][1] = "I";					// I
        table["I"][2] = "\xc3\x8d";				// Í
        table["I"][3] = "\xc3\x8c";				// Ì
        table["I"][4] = "I";					// I
        table["I"][5] = "\xc3\x8e";				// Î
        table["I"][6] = "\xc7\x8f";				// Ǐ
        table["I"][7] = "\xc4\xaa";				// Ī
        table["I"][8] = "I\xcc\x8d";			// I̍
        table["I"][9] = "I\xcc\x8b";			// I̋
        table["I"][kPOJ9thTone] = "\xc4\xac";	// Ĭ
        table["M"][1] = "M";					// M
        table["M"][2] = "\xe1\xb8\xbe";			// Ḿ
        table["M"][3] = "M\xcc\x80";			// M̀
        table["M"][4] = "M";					// M
        table["M"][5] = "M\xcc\x82";			// M̂
        table["M"][6] = "M\xcc\x8c";			// M̌
        table["M"][7] = "M\xcc\x84";			// M̄
        table["M"][8] = "M\xcc\x8d";			// M̍
        table["M"][9] = "M\xcc\x8b";			// M̋
        table["M"][kPOJ9thTone] = "M\xcc\x86";	// M̆
        table["N"][1] = "N";					// N
        table["N"][2] = "\xc5\x83";				// Ń
        table["N"][3] = "\xc7\xb8";				// Ǹ
        table["N"][4] = "N";					// N
        table["N"][5] = "N\xcc\x82";			// N̂
        table["N"][6] = "N\xcc\x8c";			// Ň
        table["N"][7] = "N\xcc\x84";			// N̄
        table["N"][8] = "N\xcc\x8d";			// N̍
        table["N"][9] = "N\xcc\x8b";			// N̋
        table["N"][kPOJ9thTone] = "N\xcc\x86";	// N̆
        table["O"][1] = "O";					// O
        table["O"][2] = "\xc3\x93";				// Ó
        table["O"][3] = "\xc3\x92";				// Ò
        table["O"][4] = "O";					// O
        table["O"][5] = "\xc3\x94";				// Ô
        table["O"][6] = "\xc7\x91";				// Ǒ
        table["O"][7] = "\xc5\x8c";				// Ō
        table["O"][8] = "O\xcc\x8d";			// O̍
        table["O"][9] = "\xc5\x90";				// Ő
        table["O"][kPOJ9thTone] = "\xc5\x8e";	// Ŏ
        table["Q"][1] = "O\xcd\x98";			// O͘
        table["Q"][2] = "\xc3\x93\xcd\x98";		// Ó͘
        table["Q"][3] = "\xc3\x92\xcd\x98";		// Ò͘
        table["Q"][4] = "O\xcd\x98";			// O͘
        table["Q"][5] = "\xc3\x94\xcd\x98";		// Ô͘
        table["Q"][6] = "\xc7\x91\xcd\x98";		// Ǒ͘
        table["Q"][7] = "\xc5\x8c\xcd\x98";		// Ō͘
        table["Q"][8] = "O\xcc\x8d\xcd\x98";	// O̍͘
        table["Q"][9] = "O\xcc\x8b\xcd\x98";	// Ő͘
        table["Q"][kPOJ9thTone] = "\xc5\x8e\xcd\x98";	// Ŏ͘
        table["Z"][1] = "O\u0358";
        table["Z"][2] = "O\u0358\u0301";
        table["Z"][3] = "O\u0358\u0300";
        table["Z"][4] = "O\u0358";
        table["Z"][5] = "O\u0358\u0302";
        table["Z"][6] = "O\u0358\u030c";
        table["Z"][7] = "O\u0358\u0304";
        table["Z"][8] = "O\u0358\u030d";
        table["Z"][9] = "O\u0358\u030b";
        table["Z"][kPOJ9thTone] = "O\u0358\u0306";
        table["U"][1] = "U";					// U
        table["U"][2] = "\xc3\x9a";				// Ú
        table["U"][3] = "\xc3\x99";				// Ù
        table["U"][4] = "U";					// U
        table["U"][5] = "\xc3\x9b";				// Û
        table["U"][6] = "\xc7\x93";				// Ǔ
        table["U"][7] = "\xc5\xaa";				// Ū
        table["U"][8] = "U\xcc\x8d";			// U̍
        table["U"][9] = "\xc5\xb0";				// Ű
        table["U"][kPOJ9thTone] = "\xc5\xac";	// Ŭ    

        table["ii"][1] = "\xe1\xb9\xb3";			// ṳ
        table["ii"][2] = "\xe1\xb9\xb3\xcc\x81";	// ṳ́
        table["ii"][3] = "\xe1\xb9\xb3\xcc\x80";	// ṳ̀
        table["ii"][4] = "\xe1\xb9\xb3";			// ṳ
        table["ii"][5] = "\xe1\xb9\xb3\xcc\x82";	// ṳ̂
        table["ii"][6] = "\xe1\xb9\xb3\xcc\x8c";	// ṳ̌
        table["ii"][7] = "\xe1\xb9\xb3\xcc\x84";	// ṳ̄
        table["ii"][8] = "\xe1\xb9\xb3\xcc\x8d";	// ṳ̍
        table["ii"][9] = "\xe1\xb9\xb3\xcc\x8b";	// ṳ̋
        table["ii"][kPOJ9thTone] = "\xe1\xb9\xb3\xcc\x86";	// ṳ̆
        table["II"][1] = "\xe1\xb9\xb2";			// Ṳ
        table["II"][2] = "\xe1\xb9\xb2\xcc\x81";	// Ṳ́
        table["II"][3] = "\xe1\xb9\xb2\xcc\x80";	// Ṳ̀
        table["II"][4] = "\xe1\xb9\xb2";			// Ṳ
        table["II"][5] = "\xe1\xb9\xb2\xcc\x82";	// Ṳ̂
        table["II"][6] = "\xe1\xb9\xb2\xcc\x8c";	// Ṳ̌
        table["II"][7] = "\xe1\xb9\xb2\xcc\x84";	// Ṳ̄
        table["II"][8] = "\xe1\xb9\xb2\xcc\x8d";	// Ṳ̍
        table["II"][9] = "\xe1\xb9\xb2\xcc\x8b";	// Ṳ̋
        table["II"][kPOJ9thTone] = "\xe1\xb9\xb2\xcc\x86";	// Ṳ̆        
    }
    
    if (!c_toUppercasedVowelTable) {        
        c_toUppercasedVowelTable = new map<string, string>;
        map<string, string>& table = *c_toUppercasedVowelTable; 

        map<string, map<unsigned int, string> >& vtab = *c_vowelTable;
        string lowercased = "aeimnoqu";
        for (string::const_iterator i = lowercased.begin() ; i != lowercased.end() ; ++i) {
            string lowerHead = string(1, *i);
            string upperHead = string(1, toupper(*i));
            
            for (unsigned int j = 1 ; j <= 9 ; j++) {
                string lower = vtab[lowerHead][j];
                string upper = vtab[upperHead][j];
                table[lower] = upper;
            }

            table[vtab[lowerHead][kPOJ9thTone]] = vtab[upperHead][kPOJ9thTone];
        }
    }
}

void VowelHelper::addStringToState(const string& s, const string &repSymbol, unsigned int repTone, VowelDecompositionState& startState)
{
    if (!s.length()) {
        return;        
    }
    
    VowelDecompositionState* st = &startState;
    
    for (string::const_iterator i = s.begin() ; i != s.end() ; ++i) {
        st = &(st->transitions[*i]);
    }
    
    st->representedSymbol = repSymbol;
    st->representedTone = repTone;
}

void VowelHelper::populateDecompositionStateMachine()
{
    if (c_groundState) {
        return;
    }
    
    c_groundState = new VowelDecompositionState;

    addStringToState("\xc3\xa1", "a", 2, *c_groundState); // á
    addStringToState("\xc3\xa0", "a", 3, *c_groundState); // à
    addStringToState("\xc3\xa2", "a", 5, *c_groundState); // â
    addStringToState("\xc7\x8e", "a", 6, *c_groundState); // ǎ
    addStringToState("\xc4\x81", "a", 7, *c_groundState); // ā
    addStringToState("a\xcc\x8d", "a", 8, *c_groundState); // a̍
    addStringToState("a\xcc\x8b", "a", 9, *c_groundState); // a̋
    addStringToState("\xc4\x83", "a", 9, *c_groundState); // ă */
    addStringToState("\xc3\xa9", "e", 2, *c_groundState); // é
    addStringToState("\xc3\xa8", "e", 3, *c_groundState); // è
    addStringToState("\xc3\xaa", "e", 5, *c_groundState); // ê
    addStringToState("\xc4\x9b", "e", 6, *c_groundState); // ě
    addStringToState("\xc4\x93", "e", 7, *c_groundState); // ē
    addStringToState("e\xcc\x8d", "e", 8, *c_groundState); // e̍
    addStringToState("e\xcc\x8b", "e", 9, *c_groundState); // e̋
    addStringToState("\xc4\x95", "e", 9, *c_groundState); // ĕ
    addStringToState("\xc3\xad", "i", 2, *c_groundState); // í
    addStringToState("\xc3\xac", "i", 3, *c_groundState); // ì
    addStringToState("\xc3\xae", "i", 5, *c_groundState); // î
    addStringToState("\xc7\x90", "i", 6, *c_groundState); // ǐ
    addStringToState("\xc4\xab", "i", 7, *c_groundState); // ī
    addStringToState("i\xcc\x8d", "i", 8, *c_groundState); // i̍
    addStringToState("i\xcc\x8b", "i", 9, *c_groundState); // i̋
    addStringToState("\xc4\xad", "i", 9, *c_groundState); // ĭ
    addStringToState("\xe1\xb8\xbf", "m", 2, *c_groundState); // ḿ
    addStringToState("m\xcc\x80", "m", 3, *c_groundState); // m̀
    addStringToState("m\xcc\x82", "m", 5, *c_groundState); // m̂
    addStringToState("m\xcc\x8c", "m", 6, *c_groundState); // m̌
    addStringToState("m\xcc\x84", "m", 7, *c_groundState); // m̄
    addStringToState("m\xcc\x8d", "m", 8, *c_groundState); // m̍
    addStringToState("m\xcc\x8b", "m", 9, *c_groundState); // m̋
    addStringToState("m\xcc\x86", "m", 9, *c_groundState); // m̆
    addStringToState("\xc5\x84", "n", 2, *c_groundState); // ń
    addStringToState("\xc7\xb9", "n", 3, *c_groundState); // ǹ
    addStringToState("n\xcc\x82", "n", 5, *c_groundState); // n̂
    addStringToState("n\xcc\x8c", "n", 6, *c_groundState); // ň
    addStringToState("n\xcc\x84", "n", 7, *c_groundState); // n̄
    addStringToState("n\xcc\x8d", "n", 8, *c_groundState); // n̍
    addStringToState("n\xcc\x8b", "n", 9, *c_groundState); // n̋
    addStringToState("n\xcc\x86", "n", 9, *c_groundState); // n̆
    addStringToState("\xc3\xb3", "o", 2, *c_groundState); // ó
    addStringToState("\xc3\xb2", "o", 3, *c_groundState); // ò
    addStringToState("\xc3\xb4", "o", 5, *c_groundState); // ô
    addStringToState("\xc7\x92", "o", 6, *c_groundState); // ǒ
    addStringToState("\xc5\x8d", "o", 7, *c_groundState); // ō
    addStringToState("o\xcc\x8d", "o", 8, *c_groundState); // o̍
    addStringToState("\xc5\x91", "o", 9, *c_groundState); // ő
    addStringToState("\xc5\x8f", "o", 9, *c_groundState); // ŏ
    addStringToState("o\xcd\x98", "oo", 1, *c_groundState); // o͘
    addStringToState("\xc3\xb3\xcd\x98", "oo", 2, *c_groundState); // ó͘
    addStringToState("\xc3\xb2\xcd\x98", "oo", 3, *c_groundState); // ò͘
    addStringToState("\xc3\xb4\xcd\x98", "oo", 5, *c_groundState); // ô͘
    addStringToState("\xc7\x92\xcd\x98", "oo", 6, *c_groundState); // ǒ͘
    addStringToState("\xc5\x8d\xcd\x98", "oo", 7, *c_groundState); // ō͘
    addStringToState("o\xcc\x8d\xcd\x98", "oo", 8, *c_groundState); // o̍͘
    addStringToState("o\xcc\x8b\xcd\x98", "oo", 9, *c_groundState); // ő͘
    addStringToState("\xc5\x8f\xcd\x98", "oo", 9, *c_groundState); // ŏ͘
    addStringToState("\xc3\xba", "u", 2, *c_groundState); // ú
    addStringToState("\xc3\xb9", "u", 3, *c_groundState); // ù
    addStringToState("\xc3\xbb", "u", 5, *c_groundState); // û
    addStringToState("\xc7\x94", "u", 6, *c_groundState); // ǔ
    addStringToState("\xc5\xab", "u", 7, *c_groundState); // ū
    addStringToState("u\xcc\x8d", "u", 8, *c_groundState); // u̍
    addStringToState("\xc5\xb1", "u", 9, *c_groundState); // ű
    addStringToState("\xc5\xad", "u", 9, *c_groundState); // ŭ
    addStringToState("\xc3\x81", "A", 2, *c_groundState); // Á
    addStringToState("\xc3\x80", "A", 3, *c_groundState); // À
    addStringToState("\xc3\x82", "A", 5, *c_groundState); // Â
    addStringToState("\xc7\x8d", "A", 6, *c_groundState); // Ǎ
    addStringToState("\xc4\x80", "A", 7, *c_groundState); // Ā
    addStringToState("A\xcc\x8d", "A", 8, *c_groundState); // A̍
    addStringToState("A\xcc\x8b", "A", 9, *c_groundState); // A̋
    addStringToState("\xc4\x82", "A", 9, *c_groundState); // Ă
    addStringToState("\xc3\x89", "E", 2, *c_groundState); // É
    addStringToState("\xc3\x88", "E", 3, *c_groundState); // È
    addStringToState("\xc3\x8a", "E", 5, *c_groundState); // Ê
    addStringToState("\xc4\x9a", "E", 6, *c_groundState); // Ě
    addStringToState("\xc4\x92", "E", 7, *c_groundState); // Ē
    addStringToState("E\xcc\x8d", "E", 8, *c_groundState); // E̍
    addStringToState("E\xcc\x8b", "E", 9, *c_groundState); // E̋
    addStringToState("\xc4\x94", "E", 9, *c_groundState); // Ĕ
    addStringToState("\xc3\x8d", "I", 2, *c_groundState); // Í
    addStringToState("\xc3\x8c", "I", 3, *c_groundState); // Ì
    addStringToState("\xc3\x8e", "I", 5, *c_groundState); // Î
    addStringToState("\xc7\x8f", "I", 6, *c_groundState); // Ǐ
    addStringToState("\xc4\xaa", "I", 7, *c_groundState); // Ī
    addStringToState("I\xcc\x8d", "I", 8, *c_groundState); // I̍
    addStringToState("I\xcc\x8b", "I", 9, *c_groundState); // I̋
    addStringToState("\xc4\xac", "I", 9, *c_groundState); // Ĭ
    addStringToState("\xe1\xb8\xbe", "M", 2, *c_groundState); // Ḿ
    addStringToState("M\xcc\x80", "M", 3, *c_groundState); // M̀
    addStringToState("M\xcc\x82", "M", 5, *c_groundState); // M̂
    addStringToState("M\xcc\x8c", "M", 6, *c_groundState); // M̌
    addStringToState("M\xcc\x84", "M", 7, *c_groundState); // M̄
    addStringToState("M\xcc\x8d", "M", 8, *c_groundState); // M̍
    addStringToState("M\xcc\x8b", "M", 9, *c_groundState); // M̋
    addStringToState("M\xcc\x86", "M", 9, *c_groundState); // M̆
    addStringToState("\xc5\x83", "N", 2, *c_groundState); // Ń
    addStringToState("\xc7\xb8", "N", 3, *c_groundState); // Ǹ
    addStringToState("N\xcc\x82", "N", 5, *c_groundState); // N̂
    addStringToState("N\xcc\x8c", "N", 6, *c_groundState); // Ň
    addStringToState("N\xcc\x84", "N", 7, *c_groundState); // N̄
    addStringToState("N\xcc\x8d", "N", 8, *c_groundState); // N̍
    addStringToState("N\xcc\x8b", "N", 9, *c_groundState); // N̋
    addStringToState("N\xcc\x86", "N", 9, *c_groundState); // N̆
    addStringToState("\xc3\x93", "O", 2, *c_groundState); // Ó
    addStringToState("\xc3\x92", "O", 3, *c_groundState); // Ò
    addStringToState("\xc3\x94", "O", 5, *c_groundState); // Ô
    addStringToState("\xc7\x91", "O", 6, *c_groundState); // Ǒ
    addStringToState("\xc5\x8c", "O", 7, *c_groundState); // Ō
    addStringToState("O\xcc\x8d", "O", 8, *c_groundState); // O̍
    addStringToState("\xc5\x90", "O", 9, *c_groundState); // Ő
    addStringToState("\xc5\x8e", "O", 9, *c_groundState); // Ŏ
    addStringToState("O\xcd\x98", "Oo", 1, *c_groundState); // O͘
    addStringToState("\xc3\x93\xcd\x98", "Oo", 2, *c_groundState); // Ó͘
    addStringToState("\xc3\x92\xcd\x98", "Oo", 3, *c_groundState); // Ò͘
    addStringToState("\xc3\x94\xcd\x98", "Oo", 5, *c_groundState); // Ô͘
    addStringToState("\xc7\x91\xcd\x98", "Oo", 6, *c_groundState); // Ǒ͘
    addStringToState("\xc5\x8c\xcd\x98", "Oo", 7, *c_groundState); // Ō͘
    addStringToState("O\xcc\x8d\xcd\x98", "Oo", 8, *c_groundState); // O̍͘
    addStringToState("O\xcc\x8b\xcd\x98", "Oo", 9, *c_groundState); // Ő͘
    addStringToState("\xc5\x8e\xcd\x98", "Oo", 9, *c_groundState); // Ŏ͘
    addStringToState("\xc3\x9a", "U", 2, *c_groundState); // Ú
    addStringToState("\xc3\x99", "U", 3, *c_groundState); // Ù
    addStringToState("\xc3\x9b", "U", 5, *c_groundState); // Û
    addStringToState("\xc7\x93", "U", 6, *c_groundState); // Ǔ
    addStringToState("\xc5\xaa", "U", 7, *c_groundState); // Ū
    addStringToState("U\xcc\x8d", "U", 8, *c_groundState); // U̍
    addStringToState("\xc5\xb0", "U", 9, *c_groundState); // Ű
    addStringToState("\xc5\xac", "U", 9, *c_groundState); // Ŭ

    // oldish POJ "middle-dot" ou
    addStringToState("o\xc2\xb7", "oo", 1, *c_groundState); // o·
    addStringToState("\xc3\xb3\xc2\xb7", "oo", 2, *c_groundState); // ó·
    addStringToState("\xc3\xb2\xc2\xb7", "oo", 3, *c_groundState); // ò·
    addStringToState("\xc3\xb4\xc2\xb7", "oo", 5, *c_groundState); // ô·
    addStringToState("\xc7\x92\xc2\xb7", "oo", 6, *c_groundState); // ǒ·
    addStringToState("\xc5\x8d\xc2\xb7", "oo", 7, *c_groundState); // ō·
    addStringToState("o\xcc\x8d\xc2\xb7", "oo", 8, *c_groundState); // o̍·
    addStringToState("\xc5\x91\xc2\xb7", "oo", 9, *c_groundState); // ő·
    addStringToState("\xc5\x8f\xc2\xb7", "oo", 9, *c_groundState); // ŏ·
    addStringToState("O\xc2\xb7", "Oo", 1, *c_groundState); // O·
    addStringToState("\xc3\x93\xc2\xb7", "Oo", 2, *c_groundState); // Ó·
    addStringToState("\xc3\x92\xc2\xb7", "Oo", 3, *c_groundState); // Ò·
    addStringToState("\xc3\x94\xc2\xb7", "Oo", 5, *c_groundState); // Ô·
    addStringToState("\xc7\x91\xc2\xb7", "Oo", 6, *c_groundState); // Ǒ·
    addStringToState("\xc5\x8c\xc2\xb7", "Oo", 7, *c_groundState); // Ō·
    addStringToState("O\xcc\x8d\xc2\xb7", "Oo", 8, *c_groundState); // O̍·
    addStringToState("\xc5\x90\xc2\xb7", "Oo", 9, *c_groundState); // Ő·
    addStringToState("\xc5\x8e\xc2\xb7", "Oo", 9, *c_groundState); // Ŏ·

    // Hakka
    addStringToState("\xe1\xb9\xb3", "ii", 1, *c_groundState); // ṳ
    addStringToState("\xe1\xb9\xb3\xcc\x81", "ii", 2, *c_groundState); // ṳ́
    addStringToState("\xe1\xb9\xb3\xcc\x80", "ii", 3, *c_groundState); // ṳ̀
    addStringToState("\xe1\xb9\xb3\xcc\x82", "ii", 5, *c_groundState); // ṳ̂
    addStringToState("\xe1\xb9\xb3\xcc\x8c", "ii", 6, *c_groundState); // ṳ̌
    addStringToState("\xe1\xb9\xb3\xcc\x84", "ii", 7, *c_groundState); // ṳ̄
    addStringToState("\xe1\xb9\xb3\xcc\x8d", "ii", 8, *c_groundState); // ṳ̍
    addStringToState("\xe1\xb9\xb3\xcc\x8b", "ii", 9, *c_groundState); // ṳ̋
    addStringToState("\xe1\xb9\xb3\xcc\x86", "ii", 9, *c_groundState); // ṳ̆
    addStringToState("\xe1\xb9\xb2", "Ii", 1, *c_groundState); // Ṳ
    addStringToState("\xe1\xb9\xb2\xcc\x81", "Ii", 2, *c_groundState); // Ṳ́
    addStringToState("\xe1\xb9\xb2\xcc\x80", "Ii", 3, *c_groundState); // Ṳ̀
    addStringToState("\xe1\xb9\xb2\xcc\x82", "Ii", 5, *c_groundState); // Ṳ̂
    addStringToState("\xe1\xb9\xb2\xcc\x8c", "Ii", 6, *c_groundState); // Ṳ̌
    addStringToState("\xe1\xb9\xb2\xcc\x84", "Ii", 7, *c_groundState); // Ṳ̄
    addStringToState("\xe1\xb9\xb2\xcc\x8d", "Ii", 8, *c_groundState); // Ṳ̍
    addStringToState("\xe1\xb9\xb2\xcc\x8b", "Ii", 9, *c_groundState); // Ṳ̋
    addStringToState("\xe1\xb9\xb2\xcc\x86", "Ii", 9, *c_groundState); // Ṳ̆
}

VowelDecompositionState::VowelDecompositionState()
                        : representedTone(0)
{
}
