# Formosana: A C++ Library for Processing Taiwanese Languages

This is a C++ library for processing languages spoken in Taiwan. Currently it covers Taiwanese Mandarin, Holo and Hakka. The library has been used in real-life input method and language processing tool development.


## Synopsis

In `Examples/Overview` there is a sample project with a simple Makefile. It should build on most UNIX systems, including Mac OS X. Just type `make` and press enter.

If you are on Windows, a Visual Studio solution file, Overview.sln, with all necessary project files, is also supplied in the same directory. You should be able to build it with Visual Studio 2008 or Visual C++ 2008 Express Edition without any trouble. Note that the built binary is a Win32 console application with output in UTF-8. It's probably wise to pipe the output from stdout to a file and read it with a Unicode-compliant text editor. 64-bit compatibility is also checked.

Take a quick look at `MandarinDemo.cpp` and `RomanizationDemo.cpp` and you'll find brief snippets on what this library can do.

For example, you can build Mandarin syllables using the Bopomofo system, and convert the syllable to either composed Bopomofo stirng, or to Hanyu Pinyin:

    // create a Mandarin syllable, compose the "mian4" (face) syllable
    BopomofoSyllable m(BopomofoSyllable::M), i(BopomofoSyllable::I), an(BopomofoSyllable::AN), tone4(BopomofoSyllable::Tone4);

    BopomofoSyllable mian4;
    mian4 += m;     // now mian4 = ㄇ
    mian4 += i;     // now mian4 = ㄇㄧ
    mian4 += an;    // now mian4 = ㄇㄧㄢ
    mian4 += tone4; // now mian4 = ㄇㄧㄢˋ

    // Output a composed Bopomofo string (UTF-8)
    // expected output: mian4 = ㄇㄧㄢˋ
    cout << "mian4 = " << mian4.composedString() << endl;

    // Output a Hanyu Pinyin string
    // expected output: mian4 = mian4
    cout << "mian4 = " << mian4.HanyuPinyinString(true, true) << endl;

Conversions from the reverse direction (composed Bopomofo or Hanyu Pinyin to syllable) are also provided. Note, though, the library currently doesn't take accented form of Hanyu Pinyin. So "lan2" is ok, "lán" is not.

Holo and Hakka languages are supported via a shared class called TaiwaneseRomanization. The design explains further explanation, and please ask me why the class is designed as such. In short, it is aimed for input method development. This being said, it can serve general language processing purposes well. Examples:

    // create a POJ syllable for chioh8 (stone), then convert to TL
    RomanizationSyllable s1;
    s1.setInputType(POJSyllable);    
    s1.insertCharacterAtCursor('c');
    s1.insertCharacterAtCursor('h');
    s1.insertCharacterAtCursor('i');
    s1.insertCharacterAtCursor('o', 8); // enter a tone here
    s1.insertCharacterAtCursor('h');
    
    //
    // expected output: 
    // POJ syllable "chio̍h" converts to TL => "tsio̍h"
    // TL syllable "puánn" converts to POJ => "poáⁿ"
    //
    cout << "POJ syllable \"" << s1.composedForm() 
        << "\" converts to TL => \"" << s1.convertToTLSyllable().composedForm() << "\"" << endl;
    
    // transforms a composed string back to its normalized database (ASCII) form
    string composedString = "chio\xcc\x8dh"; // chio̍h
    string databaseForm = VowelHelper::queryFormFromComposedForm(composedString);
    
    //
    // expected output:
    // Composed form "chio̍h" to ASCII form => "chioh8"
    //
    cout << "Composed form \"" << composedString << "\" to ASCII form => \"" << databaseForm << "\"" << endl;

    
    // Converts Hakka "sṳ" (the last syllable in "Pha̍k-fa-sṳ") back to the query form
    string hakkaString = "s\xe1\xb9\xb3";   // sṳ
    string hSQF = VowelHelper::queryFormFromComposedForm(hakkaString);
    
    //
    // expected output:
    // Composed form "sṳ" to ASCII form => "sii"
    //
    cout << "Composed form \"" << hakkaString << "\" to ASCII form => \"" << hSQF << "\"" << endl;


## External Dependencies

This library is designed to be as much self-containing as possible. Still, it comes with a small portion of the [OpenVanilla Framework](http://github.com/lukhnos/openvanilla-renaissance) on which the library depends. The parts really used are UTF-8 helpers, wildcard objects (a simplified regex implementation) and string helpers. No other external dependency is required, and to work with this library, the files presented here are all you need.


## History and Acknowledgment

This project was originated from OpenVanilla's POJ module, first developed in late 2004 with the advice of Professor Pektiong Tan of National Tsing Hua University, Hsinchu, Taiwan. In late 2005, Professor Tan and I started revising the module, and had drafted out, in rough BNF notation, a phonetic grammar of the Holo language that could be used in a syllabizing library. The drafted grammar didn't go anywhere, but the rules have been fully incorporated in this library.

In late 2006, the project has evolved into [lib-formosa](http://code.google.com/p/lib-formosa), the immediate predecessor of this project. I started to rework OpenVanilla's Bopomofo module in late 2007, and the result has become the present Mandarin library.

The two components, Taiwanese Romanization support and Mandarin, are finally merged into one coherent code base in early 2010.


## Limitations

While I would certainly like the library to cover as many Romanization systems as possible, the present implementation reflects my limited knowledge in the field (and limited time to work on this project). The library is designed to be highly extendable, so it should be relatively simple to incorporate other Romanization systems and keyboard layouts. I'd be happy to receive your pull requests and fully functional patches.


## Copyright and License

Copyright (c) 2006-2010 Lukhnos D. Liu.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    