//
// RomanizationDemo.cpp
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

#include "RomanizationDemo.h"
#include "Formosana.h"
#include <iostream>

using namespace Formosa::TaiwaneseRomanization;
using namespace std;

void RunRomanizationDemo()
{
    cout << endl << endl << "======= Taiwanese Romanization demo" << endl;
    
    // create a POJ syllable for chioh8 (stone), then convert to TL
    RomanizationSyllable s1;
    s1.setInputType(POJSyllable);    
    s1.insertCharacterAtCursor('c');
    s1.insertCharacterAtCursor('h');
    s1.insertCharacterAtCursor('i');
    s1.insertCharacterAtCursor('o', 8); // enter a tone here
    s1.insertCharacterAtCursor('h');
    cout << "POJ syllable \"" << s1.composedForm()
        << "\" converts to TL => \"" << s1.convertToTLSyllable().composedForm() << "\"" << endl;
    
    // create a TL syllable for puann2 (foundation), then convert to TL
    RomanizationSyllable s2;
    s2.setInputType(TLSyllable);
    s2.insertCharacterAtCursor('p');
    s2.insertCharacterAtCursor('u');
    s2.insertCharacterAtCursor('a');
    s2.insertCharacterAtCursor('n');
    s2.insertCharacterAtCursor('n');
    s2.normalize(2);
    cout << "TL syllable \"" << s2.composedForm()
        << "\" converts to POJ => \"" << s2.convertToPOJSyllable().composedForm() << "\"" << endl;
    
    // transforms a composed string back to its normalized database (ASCII) form
    string composedString = "chio\xcc\x8dh"; // chio̍h
    string databaseForm = VowelHelper::queryFormFromComposedForm(composedString);    
    cout << "Composed form \"" << composedString << "\" to ASCII form => \"" << databaseForm << "\"" << endl;
    
    // Also composes the Hakka "sṳ" (the last syllable in "Pha̍k-fa-sṳ")
    RomanizationSyllable h1;
    h1.setInputType(POJSyllable);
    h1.insertCharacterAtCursor('s');
    h1.insertCharacterAtCursor('i');
    h1.insertCharacterAtCursor('i');
    cout << "Hakka syllable sii => " << h1.composedForm() << endl;
    
    // Converts Hakka "sṳ" (the last syllable in "Pha̍k-fa-sṳ") back to the query form
    string hakkaString = "s\xe1\xb9\xb3";   // sṳ
    string hSQF = VowelHelper::queryFormFromComposedForm(hakkaString);
    cout << "Composed form \"" << hakkaString << "\" to ASCII form => \"" << hSQF << "\"" << endl;
}
