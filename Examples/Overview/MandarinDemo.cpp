//
// MandarinDemo.cpp
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

#include "MandarinDemo.h"
#include "Formosana.h"
#include <iostream>

using namespace Formosa::Mandarin;
using namespace std;

void RunMandarinDemo()
{
    cout << endl << endl << "======= Mandarin demo" << endl;
    
    // create a Mandarin syllable, compose the "mian4" (face) syllable
    BopomofoSyllable m(BopomofoSyllable::M), i(BopomofoSyllable::I), an(BopomofoSyllable::AN), tone4(BopomofoSyllable::Tone4);
    
    BopomofoSyllable mian4;
    mian4 += m;
    mian4 += i;
    mian4 += an;
    mian4 += tone4;
    
    // Output a composed Bopomofo string (UTF-8)
    cout << "mian4 = " << mian4.composedString() << endl;
    
    // Output a Hanyu Pinyin string
    cout << "mian4 = " << mian4.HanyuPinyinString(true, true) << endl;
    
    string zhu4Bopomofo = "\xe3\x84\x93\xe3\x84\xa8\xcb\x8b"; // ㄓㄨˋ
    string zhu4Pinyin = "zhu4";
    
    // create two instances of zhu4 and then compare them
    BopomofoSyllable zhu4_1 = BopomofoSyllable::FromComposedString(zhu4Bopomofo);
    BopomofoSyllable zhu4_2 = BopomofoSyllable::FromHanyuPinyin(zhu4Pinyin);    
    cout << "zhu4 = " << zhu4_1.HanyuPinyinString(true, true) 
        << " = " << zhu4_2.composedString()
        << ", the two are equal? " << ((zhu4_1 == zhu4_2) ? "yes" : "no") 
        << endl;
        
    // Bopomofo/Phonetics input method keyboard layout demo    
    BopomofoReadingBuffer buf1(BopomofoKeyboardLayout::StandardLayout());
    BopomofoReadingBuffer buf2(BopomofoKeyboardLayout::ETenLayout());
    BopomofoReadingBuffer buf3(BopomofoKeyboardLayout::HanyuPinyinLayout());
    
    // On standard Taiwanese US keyboard, the keystrokes 'q', '0', '6' yield "pan2"
    buf1.combineKey('q');
    buf1.combineKey('0');
    buf1.combineKey('6');
    cout << "standard layout q06 => " << buf1.syllable().composedString() << endl;

    // On ETen keyboard layout, 'p', '8', '2' yield "pan2"
    buf2.combineKey('p');
    buf2.combineKey('8');
    buf2.combineKey('2');
    cout << "ETen layout p82 => " << buf2.syllable().composedString() << endl;
    
    // Hanyu Pinyin
    buf3.combineKey('p');
    buf3.combineKey('a');
    buf3.combineKey('n');
    buf3.combineKey('2');
    cout << "Hanyu Pinyin pan2 => " << buf3.syllable().composedString() << endl;
}
