//
// TaiwaneseRomanization.cpp
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

#include "TaiwaneseRomanization.h"

using namespace Formosa::TaiwaneseRomanization;

Composable::~Composable()
{
}

ComposableStringBuffer::ComposableStringBuffer() : _cursor(0)
{
}

unsigned int ComposableStringBuffer::cursor()
{
    return _cursor;
}

unsigned int ComposableStringBuffer::setCursor(unsigned int c)
{
    if (c <= numberOfCodepoints()) _cursor = c;
    return _cursor;
}

void ComposableStringBuffer::clear()
{
    _cursor = 0;
    strvec.clear();
}

bool ComposableStringBuffer::empty()
{
    return strvec.empty();
}

unsigned int ComposableStringBuffer::numberOfCodepoints()
{
    return strvec.size();
}

bool ComposableStringBuffer::insertCharacterAt(unsigned int i, char c)
{
    if (i > numberOfCodepoints()) return false;
    strvec.insert(strvec.begin()+i, string(1, c));
    return true;
}

bool ComposableStringBuffer::removeCodepointAt(unsigned int i)
{
    if (i >= numberOfCodepoints()) return false;
    strvec.erase(strvec.begin() + i);
    return true;
}

const string ComposableStringBuffer::composedForm()
{
    return internalForm();
}

const string ComposableStringBuffer::internalForm()
{
    string newstr;
    unsigned int s=numberOfCodepoints();
    for (unsigned int i=0; i<s; i++) newstr+=strvec[i];
    return newstr;
}

RomanizationSymbol::RomanizationSymbol() : _tone(0), _type(POJSyllable)
{
}

RomanizationSymbol::RomanizationSymbol(const string &s, SyllableType t) : _tone(0), _type(t), _symbol(s)
{
}

RomanizationSymbol::RomanizationSymbol(const RomanizationSymbol &s) : _tone(s._tone), _type(s._type), _symbol(s._symbol)
{
}

void RomanizationSymbol::setType(SyllableType t)
{
    _type = t;
}

const RomanizationSymbol& RomanizationSymbol::operator=(const RomanizationSymbol &s)
{
    _symbol = s._symbol;
    _tone = s._tone;
    _type = s._type;
    return *this;
}

const string RomanizationSymbol::symbol() const
{
    return string(_symbol);
}

const string RomanizationSymbol::symbolInLowerCase() const
{
    string lower;
    unsigned int s=_symbol.length();
    for (unsigned int i=0; i<s; i++) lower+=tolower(_symbol[i]);
    return lower;
}

const string RomanizationSymbol::setSymbol(const string& s)
{
    return (_symbol = s);
}

const string RomanizationSymbol::composedForm(bool forcePOJStyle) const
{
    bool usePOJStyleOUAndNN = (_type == POJSyllable) || (_type == HakkaPFSSyllable) || forcePOJStyle;
    bool usePOJStyleNinthToneMark = (_type == POJSyllable);
    bool composeII = (_type == HakkaPFSSyllable);
    
    
    unsigned int nanTone = _tone;
    if (_type == HakkaPFSSyllable) {
        switch (_tone) {
            case 1: nanTone = 5; break;
            case 2: nanTone = 3; break;
            case 3: nanTone = 2; break;
            case 4: nanTone = 4; break;
            case 5: nanTone = 8; break;
            case 6: nanTone = 1; break;                
        }
    }
    
    string composed = VowelHelper::symbolForVowel(_symbol, nanTone, usePOJStyleOUAndNN, usePOJStyleNinthToneMark, composeII);
    if (!composed.length()) return _symbol;
    return composed;
}

unsigned int RomanizationSymbol::composedLength() const
{
    string composed = composedForm();
    unsigned int len = 0, clen = composed.length();
    for (unsigned int i=0; i<clen; )
    {
        if (!(composed[i] & 0x80)) {
            len++;
            i++;
        }
        else if ((composed[i] & 0xe0) == 0xc0) {
            len++;
            i+=2;
        }
        else if ((composed[i] & 0xf0) == 0xe0) {
            len++;
            i+=3;
        }
        else {
            len++;
            i+=4;
        }
    }
    
    // fprintf (stderr, "composed=%s, strlen=%d, calculated len=%d\n", composed.c_str(), clen, len);
    
    return len;
}

unsigned int RomanizationSymbol::tone() const
{
    return _tone;
}

unsigned int RomanizationSymbol::setTone(unsigned int t)
{
    _tone = t > 9 ? _tone : t;
    return _tone;
}

bool RomanizationSymbol::isUpperCase() const
{
    if (!_symbol.length()) return false;
    return toupper(_symbol[0]) == _symbol[0];
}



RomanizationSyllable::RomanizationSyllable() : _inputType(POJSyllable), _inputOption(DiacriticGivenBeforeVowel), 
_forcePOJStyle(false),
_cursor(0), _preparedTone(0)
{
}

RomanizationSyllable::RomanizationSyllable(const RomanizationSyllable &s) : _inputType(s._inputType),
_inputOption(s._inputOption),
_forcePOJStyle(s._forcePOJStyle),
_symvec(s._symvec),
_cursor(s._cursor), _preparedTone(s._preparedTone)
{
}

const RomanizationSyllable& RomanizationSyllable::operator=(const RomanizationSyllable &s)
{
    _inputType = s._inputType;
    _inputOption = s._inputOption;
    _forcePOJStyle = s._forcePOJStyle;
    _symvec = s._symvec;
    _cursor = s._cursor;
    _preparedTone = s._preparedTone;
    return *this;
}		

void RomanizationSyllable::setInputType(SyllableType t)
{
    _inputType = t;
}

void RomanizationSyllable::setInputOption(DiacriticInputOption o)
{
    if (o != _inputOption) clearPreparedTone();
    _inputOption = o;
}

void RomanizationSyllable::setForcePOJStyle(bool p)
{
    _forcePOJStyle = p;
}

void RomanizationSyllable::clear()
{
    _symvec.clear();
    _cursor = 0;
    _preparedTone = 0;
}

bool RomanizationSyllable::empty() const
{
    return _symvec.empty();
}

unsigned int RomanizationSyllable::numberOfCodepoints() const
{
    return _symvec.size();
}

const string RomanizationSyllable::composedForm()
{
    unsigned int s = _symvec.size();
    string composed;
    unsigned int i;
    
    if (_preparedTone) _cursor--;
    
    for (i=0; i<_cursor; i++) 
    {
        composed += _symvec[i].composedForm(_forcePOJStyle);
        // fprintf(stderr, "%d, symbol=%s, composed=%s, composd form=%s\n", i, _symvec[i].symbol().c_str(), _symvec[i].composedForm().c_str(), composed.c_str());
    }
    
    
    char diacriticShorthand = VowelHelper::diacriticShorthandFromTone(_preparedTone);
    
    if (diacriticShorthand) {
        composed += diacriticShorthand;
    }
    
    // fprintf(stderr, "composd form=%s\n", composed.c_str());
    
    for (; i<s; i++)
    {
        composed += _symvec[i].composedForm(_forcePOJStyle);
        // fprintf(stderr, "composd form=%s\n", composed.c_str());
    }
    
    if (_preparedTone) _cursor++;
    
    return composed;
}

void RomanizationSyllable::setCursor(unsigned int c)
{
    clearPreparedTone();
    _cursor = c;
}

unsigned int RomanizationSyllable::cursor() const
{
    unsigned int realcursor = _preparedTone ? _cursor-1 : _cursor;			
    unsigned codepointCursor=0;
    for (unsigned int i=0; i<realcursor; i++) codepointCursor+=_symvec[i].composedLength();
    
    if (_preparedTone) codepointCursor++;
    
    return codepointCursor;
}

bool RomanizationSyllable::cursorHome()
{
    clearPreparedTone();
    if (_cursor==0) return false;
    _cursor=0;
    return true;
}

bool RomanizationSyllable::cursorEnd()
{
    clearPreparedTone();
    unsigned int len = numberOfCodepoints();
    if (_cursor == len) return false;
    _cursor = len;
    return true;
}

bool RomanizationSyllable::cursorLeft()
{
    clearPreparedTone();
    if (_cursor==0) return false;
    _cursor--;
    return true;
}

bool RomanizationSyllable::cursorRight()
{
    clearPreparedTone();
    if (_cursor == numberOfCodepoints()) return false;
    _cursor++;
    return true;
}

bool RomanizationSyllable::insertSymbolAtCursor(const RomanizationSymbol &s)
{
    clearPreparedTone();
    RomanizationSymbol newsym(s);
    newsym.setType(_inputType);
    _symvec.insert(_symvec.begin() + _cursor, newsym);
    _cursor++;			
    
    return true;			
}

// if there is a prepared tone, the given tone parameter will be ignored
bool RomanizationSyllable::insertCharacterAtCursor(char c, unsigned int tone)
{
    // fprintf(stderr, "insert char %d ('%c'), cursor=%d\n", c, c, _cursor);
    if (VowelHelper::isDiacriticShorthand(c))
    {
        unsigned int tone = VowelHelper::toneFromDiacriticShorthand(c);
        
        // if there's already a prepared tone, we replace it with the current one
        if (_preparedTone) {
            _preparedTone = tone;
            return true;
        }
        
        if (_inputOption==DiacriticGivenBeforeVowel) {
            _preparedTone = tone;
            _cursor++;					
        }
        else {
            // diacritic given after vowel
            if (hasPreviousSymbolAtCursor()) previousSymbolAtCursor().setTone(tone);
        }
        return true;
    }
    
    // if it's not a diacritic symbol, it's POJ^W^W^W, and it's n or u or g,
    // (and if there's no prepared tone!)
    // we need to do something special...
    if (!VowelHelper::isDiacriticShorthand(c) && !_preparedTone /* && _inputType==POJSyllable */)
    {
        if (hasPreviousSymbolAtCursor())
        {
            string prev = previousSymbolAtCursor().symbolInLowerCase();
            
            // N -> nn only works if the first character of the syllable is not an
            // all uppercase symbol
            if (c=='N' && ((prev != "n") && (prev != "nn")) && _inputType == POJSyllable
                && (_symvec.size() > 0 && !_symvec[0].isUpperCase()))
            {
                // insert two n's in a row
                _symvec.insert(_symvec.begin() + _cursor, RomanizationSymbol(string("nn"), _inputType));
                _cursor++;
                return true;
            }
            else if (tolower(c)=='n' && prev=="n") {
                previousSymbolAtCursor().setSymbol(previousSymbolAtCursor().symbol() + string(1, c));
                return true;
            }
            else if (_inputType == POJSyllable && tolower(c)=='u' && prev=="o") {
                previousSymbolAtCursor().setSymbol(previousSymbolAtCursor().symbol() + string(1, c));
                return true;
            }
            else if (_inputType == TLSyllable && tolower(c)=='o' && prev=="o") {
                previousSymbolAtCursor().setSymbol(previousSymbolAtCursor().symbol() + string(1, c));
                return true;
            }
            else if ((_inputType == HakkaPFSSyllable) && tolower(c)=='i' && prev=="i") {
                previousSymbolAtCursor().setSymbol(previousSymbolAtCursor().symbol() + string(1, c));
                return true;
            }                    
            else if (tolower(c)=='g' && prev=="nn") {
                // we need to break them up!
                string before = previousSymbolAtCursor().symbol();
                
                // and the tone of the previous symbol (when it's combined into nn) will be retained
                previousSymbolAtCursor().setSymbol(before.substr(0, 1));
                
                // insert one n and one g
                _symvec.insert(_symvec.begin() + _cursor, RomanizationSymbol(before.substr(1,1), _inputType));
                _cursor++;
                _symvec.insert(_symvec.begin() + _cursor, RomanizationSymbol(string(1, c), _inputType));
                _cursor++;
                return true;
            }
        }
    }
    
    
    RomanizationSymbol s(string(1, c), _inputType);
    if (_preparedTone)
    {
        _cursor--;
        s.setTone(_preparedTone);
        _preparedTone = 0;
    }
    else if (tone > 1 || _inputType == HakkaPFSSyllable)
    {
        s.setTone(tone);
    }
    
    _symvec.insert(_symvec.begin() + _cursor, s);
    _cursor++;			
    
    return true;
}

bool RomanizationSyllable::removeCharacterAtRightOfCursor()	// backspace
{
    if (_preparedTone)
    {
        clearPreparedTone();
        return true;
    }
    
    if (atBeginning()) return false;
    _cursor--;
    _symvec.erase(_symvec.begin() + _cursor);
    return true;
}

bool RomanizationSyllable::removeCharacterAtLeftOfCursor()	// delete
{
    // we do some tightrope trick here: if we have _preparedTone ready,
    // we "push back" the real _cursor position, do the delete thing,
    // then push it back
    bool retval=true;
    if (_preparedTone) _cursor--;
    if (atEnd()) retval=false; else _symvec.erase(_symvec.begin() + _cursor);
    if (_preparedTone) _cursor++;
    return retval;
}

// returns a normalized string that represents the "internal form" for querying the database
// implies normalization
const string RomanizationSyllable::normalizedQueryData(unsigned int finalTone)
{
    RomanizationSyllable s(*this);
    s.normalize(finalTone);
    string query;
    
    unsigned int size=s._symvec.size();
    unsigned int loudest = 0;
    
    for (unsigned int i=0; i<size; i++) {
        query = query + s._symvec[i].symbol();
        if (s._symvec[i].tone() > 1) loudest = s._symvec[i].tone();
        // fprintf (stderr, "combining query data %s, tone %d\n", s._symvec[i].symbol().c_str(), s._symvec[i].tone());
    }
    
    if (loudest > 1) query = query + string(1, loudest+'0');
    return query;
}

static unsigned int FindVowel(vector<RomanizationSymbol>& symvec, unsigned int start)
{
    unsigned i = start >= symvec.size() ? symvec.size() : start;
    while (i < symvec.size()) {
        string s = symvec[i].symbolInLowerCase();
        
        if (s == "a" || s == "e" || s == "i" || s == "o" || s == "u" || s == "ou" || s == "oo" || s == "ii") {
            return i;
        }
        
        i++;
    }
    
    return i;
}

// normalization is an "identpotent" function, ie. the result should
// be the same no matter how many times you call it--this being a very
// important linguistic characteristic of this function	
void RomanizationSyllable::normalize(unsigned int finalTone)
{
    bool pureTL = (_inputType == TLSyllable && !_forcePOJStyle);
    
    // fprintf (stderr, "input finalTone=%d\n", finalTone);
    unsigned int end = _symvec.size();
    
    // if it's empty, just return
    if (!end) return;
    
    unsigned int loudestVowel = end;
    unsigned int loudestTone = 0;
    unsigned int p;
    
    // find the loudest vowel
#define FLV(x) ((p=findSymbol(x)) != end)
#define SETLOUDEST(v) do { loudestVowel = v; if (_symvec[loudestVowel].tone()>0) { loudestTone = _symvec[loudestVowel].tone(); } } while(0)
    
    
    if (!pureTL) {
        // do ng first
        // see if it's ng
        if ((p=findSymbolPair("n", "g")) != end) {
            SETLOUDEST(p);
        }
        else {
            // do m and n
            if (FLV("m")) SETLOUDEST(p);        
            if (FLV("n")) SETLOUDEST(p);
        }
        
        unsigned first = FindVowel(_symvec, 0);
        if (first != end) {
            SETLOUDEST(first);
            
            unsigned second = FindVowel(_symvec, first + 1);
            if (second != end && _symvec[first].symbolInLowerCase() != "a") {
                if (!(_symvec[first].symbolInLowerCase() == "e" && _symvec[second].symbolInLowerCase() == "e")) {
                    SETLOUDEST(second);
                }
            }
        }
    }
    else {
        if (end==1 && _symvec[0].symbolInLowerCase()=="m") SETLOUDEST(0);
        if (FLV("n")) SETLOUDEST(p);
        if (FLV("m")) SETLOUDEST(p);
        
        // see if it's ng
        if ((p=findSymbolPair("n", "g")) != end)
            SETLOUDEST(p);
        
        if (FLV("u")) SETLOUDEST(p);
        if (FLV("ii")) SETLOUDEST(p); // TODO: Check the rule here
        if (FLV("i")) SETLOUDEST(p);
        if (FLV("o")) SETLOUDEST(p);
        if (FLV("e")) SETLOUDEST(p);
        if (FLV("ou")) SETLOUDEST(p);
        if (FLV("oo")) SETLOUDEST(p);
        if (FLV("a")) SETLOUDEST(p);
    }
    
    // the last "ere" override
    if (end >= 3) {
        if (_symvec[end-1].symbolInLowerCase() == "e" && _symvec[end-2].symbolInLowerCase() == "r" && _symvec[end-3].symbolInLowerCase() == "e")
        {
            SETLOUDEST(end-1);
        }
    }
    
    if (loudestVowel==end) return;
    // fprintf(stderr, "found loudest vowel=%d (%s), loudest tone=%d\n", loudestVowel, _symvec[loudestVowel].symbol().c_str(), loudestTone);
    
    // finalTone overrides
    if (finalTone > 0) loudestTone = finalTone;
    
    for (unsigned int i=0; i<end; i++) _symvec[i].setTone(0);
    
    string lastSymbolStr = _symvec[end-1].symbolInLowerCase();
    
    // if the symbol is "i", and there's a next "u", we shift the vowel to "u" (TL only)    
    if (_symvec[loudestVowel].symbolInLowerCase()=="i" && pureTL)
    {
        if ((loudestVowel+1 < end) && (_symvec[loudestVowel+1].symbolInLowerCase() == "u" || _symvec[loudestVowel+1].symbolInLowerCase() == "ii")) {
            // if i follows a vowel, and the next vowel is u or ṳ, we put the accent on the succeeding vowel
            loudestVowel++;
        }
        else if (_inputType == POJSyllable && loudestVowel && (_symvec[loudestVowel-1].symbolInLowerCase() == "u" || _symvec[loudestVowel-1].symbolInLowerCase() == "ii")) {
            // if (and only if) in POJ mode/forced POJ style, and if i precedes a vowel, and the next voewl is u or ṳ, we put the accent on the preceeding vowel
            loudestVowel--;
        }
    }
    
    if (loudestTone==4 || /* loudestTone==6 || */ (_inputType != HakkaPFSSyllable && loudestTone <= 1)) {
        // ignore the 4th, 6th and 1th (or no tone), so everything is set to 0 now
        return;
    }

    unsigned int tpkhTone = (_inputType == HakkaPFSSyllable) ? 5 : 8;

    if (lastSymbolStr=="t" || lastSymbolStr=="p" || lastSymbolStr=="k" || lastSymbolStr=="h") {
        // only when the ending is t, p, k, h is the tone set -- and only when the tone is 8
        
        
        if (loudestTone==tpkhTone) _symvec[loudestVowel].setTone(loudestTone);
        return;
    }
    else {
        // if not t,p,k,h, we need to override the loudest tone--back to tone 1 !
        if (loudestTone==tpkhTone) {
            _symvec[loudestVowel].setTone(0);
            return;
        }
    }
    
    _symvec[loudestVowel].setTone(loudestTone);
    
#undef FLV
#undef SETTONE
}

char RomanizationSyllable::charAccordingToCaseOf(char c, char ref)
{
    return (ref == toupper(ref)) ? toupper(c) : tolower(c);
}

RomanizationSyllable RomanizationSyllable::convertToPOJSyllable()
{
    RomanizationSyllable syl = *this;
    syl.clearPreparedTone();
    syl.setCursor(0);
    //			if (_inputType==POJSyllable) return syl;
    
    syl.setInputType(POJSyllable);
    syl.clear();
    
    // begin TL->POJ conversion
    unsigned int size=_symvec.size();
    unsigned int i;
    
    for (i=0; i<size; i++)
    {
        RomanizationSymbol sym1 = _symvec[i];
        string str1 = sym1.symbol();
        
        // fprintf (stderr, "converting to POJ: %s\n", str1.c_str());
        
        string lowstr1 = sym1.symbolInLowerCase();
        
        // oo -> ou
        if (lowstr1=="oo")
        {
            syl.insertCharacterAtCursor(charAccordingToCaseOf('o', str1[0]), sym1.tone());
            syl.insertCharacterAtCursor(charAccordingToCaseOf('u', str1[1]));
            continue;
        }
        
        
        if (hasNextSymbol(i)) {
            RomanizationSymbol sym2 = _symvec[i+1];
            string str2 = sym2.symbol();
            string lowstr2 = sym2.symbolInLowerCase();
            
            // ou -> oo for POJ but not combined ou
            if (lowstr1=="o" && lowstr2=="u") {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('o', str1[0]), sym1.tone());
                syl.insertCharacterAtCursor(charAccordingToCaseOf('u', str2[0]));
                
                i++;
                continue;
            }
            
            
            // ts -> ch with case detection
            if (lowstr1=="t" && lowstr2=="s") {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('c', str1[0]));
                syl.insertCharacterAtCursor(charAccordingToCaseOf('h', str2[0]));
                
                i++;
                continue;
            }
            
            // ue -> oe
            if (lowstr1=="u" && lowstr2=="e") {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('o', str1[0]), sym1.tone());
                syl.insertCharacterAtCursor(charAccordingToCaseOf('e', str2[0]), sym2.tone());
                
                i++;
                continue;
            }
            
            // ua -> oa
            if (lowstr1=="u" && lowstr2=="a") {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('o', str1[0]), sym1.tone());
                syl.insertCharacterAtCursor(charAccordingToCaseOf('a', str2[0]), sym2.tone());
                
                i++;
                continue;
            }
            
            // ik -> ek (at ending)
            if (lowstr1=="i" && lowstr2=="k" && (i+2)==size) {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('e', str1[0]), sym1.tone());
                syl.insertCharacterAtCursor(charAccordingToCaseOf('k', str2[0]), sym2.tone());
                
                i++;
                continue;
            }
            
            if (hasNextNextSymbol(i) && (i+3)==size) {
                RomanizationSymbol sym3 = _symvec[i+2];
                string str3 = sym3.symbol();
                string lowstr3 = sym3.symbolInLowerCase();
                
                // ing -> eng (must be ending)					
                if (lowstr1=="i" && lowstr2=="n" && lowstr3=="g") {
                    // detect case
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('e', str1[0]), sym1.tone());
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('n', str2[0]), sym2.tone());
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('g', str3[0]), sym3.tone());
                    
                    i+=2;
                    continue;
                }
                
                // ouh -> oh (ending)
                if (lowstr1=="o" && lowstr2=="u" && lowstr3=="h") {
                    // detect case
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('o', str1[0]), sym1.tone());
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('h', str2[0]), sym2.tone());
                    
                    i+=2;
                    continue;
                }
            }			
        }
        
        syl.insertSymbolAtCursor(sym1);
    }			
    
    return syl;
}

RomanizationSyllable RomanizationSyllable::convertToTLSyllable()
{
    RomanizationSyllable syl = *this;
    syl.clearPreparedTone();
    syl.setCursor(0);
    //			if (_inputType==TLSyllable) return syl;
    
    syl.setInputType(TLSyllable);			
    syl.clear();
    
    // begin POJ->TL conversion
    unsigned int size=_symvec.size();
    unsigned int i;
    
    for (i=0; i<size; i++)
    {
        RomanizationSymbol sym1 = _symvec[i];
        string str1 = sym1.symbol();
        string lowstr1 = sym1.symbolInLowerCase();
        
        // ou -> oo
        if (lowstr1=="ou")
        {
            // detect case
            syl.insertCharacterAtCursor(charAccordingToCaseOf('o', str1[0]), sym1.tone());
            syl.insertCharacterAtCursor(charAccordingToCaseOf('o', str1[1]));
            continue;
        }
        
        
        if (hasNextSymbol(i)) {
            RomanizationSymbol sym2 = _symvec[i+1];
            string str2 = sym2.symbol();
            string lowstr2 = sym2.symbolInLowerCase();
            
            // ch -> ts with case detection
            if (lowstr1=="c" && lowstr2=="h") {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('t', str1[0]));
                syl.insertCharacterAtCursor(charAccordingToCaseOf('s', str2[0]));
                
                i++;
                continue;
            }
            
            // oe -> ue
            if (lowstr1=="o" && lowstr2=="e") {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('u', str1[0]), sym1.tone());
                syl.insertCharacterAtCursor(charAccordingToCaseOf('e', str2[0]), sym2.tone());
                
                i++;
                continue;
            }
            
            // oa -> ua
            if (lowstr1=="o" && lowstr2=="a") {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('u', str1[0]), sym1.tone());
                syl.insertCharacterAtCursor(charAccordingToCaseOf('a', str2[0]), sym2.tone());
                
                i++;
                continue;
            }
            
            // ek -> ik (at ending)
            if (lowstr1=="e" && lowstr2=="k" && (i+2)==size) {
                // detect case
                syl.insertCharacterAtCursor(charAccordingToCaseOf('i', str1[0]), sym1.tone());
                syl.insertCharacterAtCursor(charAccordingToCaseOf('k', str2[0]), sym2.tone());
                
                i++;
                continue;
            }
            
            if (hasNextNextSymbol(i) && (i+3)==size) {
                RomanizationSymbol sym3 = _symvec[i+2];
                string str3 = sym3.symbol();
                string lowstr3 = sym3.symbolInLowerCase();
                
                // ing -> eng (must be ending)					
                if (lowstr1=="e" && lowstr2=="n" && lowstr3=="g") {
                    // detect case
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('i', str1[0]), sym1.tone());
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('n', str2[0]), sym2.tone());
                    syl.insertCharacterAtCursor(charAccordingToCaseOf('g', str3[0]), sym3.tone());
                    
                    i+=2;
                    continue;
                }						
            }			
        }
        
        syl.insertSymbolAtCursor(sym1);
    }
    
    return syl;
}

bool RomanizationSyllable::atBeginning() const
{
    return _cursor == 0;
}

bool RomanizationSyllable::atEnd() const
{
    return _cursor == numberOfCodepoints();
}

void RomanizationSyllable::clearPreparedTone()
{
    if (!_preparedTone) return;
    _preparedTone = 0;
    _cursor--;
}

bool RomanizationSyllable::hasPreviousSymbolAtCursor() const
{
    unsigned int realcursor = _preparedTone ? _cursor-1 : _cursor;
    return realcursor > 0;
}

bool RomanizationSyllable::hasNextSymbol(unsigned int pos) const
{
    if (pos+1 >= _symvec.size()) return false;
    return true;
}

bool RomanizationSyllable::hasNextNextSymbol(unsigned int pos) const
{
    if (pos+2 >= _symvec.size()) return false;
    return true;
}


// the result of this function is unpredictable if there's no
// previous symbol--always check with hasPreviousSymbolAtCursor() !
RomanizationSymbol& RomanizationSyllable::previousSymbolAtCursor()
{
    unsigned int realcursor = _preparedTone ? _cursor-1 : _cursor;
    return _symvec[realcursor-1];
}

// always assumes that the given input is in all lower case
unsigned int RomanizationSyllable::findSymbol(const char *s) const
{
    string cpps(s);
    unsigned int size = _symvec.size();
    unsigned int i;
    for (i = 0; i < size; i++) {
        if (_symvec[i].symbolInLowerCase() == cpps) break;
    }
    return i;
}

unsigned int RomanizationSyllable::findSymbolPair(const char *s1, const char *s2) const
{
    string cpps1(s1), cpps2(s2);
    
    unsigned int size = _symvec.size();
    if (size < 2) return size;
    
    unsigned int i;			
    for (i = 0; i < size-1; i++) {
        if (_symvec[i].symbolInLowerCase()==cpps1 && _symvec[i+1].symbolInLowerCase()==cpps2) return i;
    }
    
    return size;
}


const RomanizationSyllable FreeFormSyllable::convertToTLFromTLPA(unsigned int finalTone)
{
    string rep=internalForm();
    RomanizationSyllable syl;
    syl.setInputType(TLSyllable);
    
    unsigned int size=rep.length();
    for (unsigned int i=0; i<size; i++)
    {
        if (rep[i]=='c') {
            syl.insertCharacterAtCursor('t');
            syl.insertCharacterAtCursor('s');
        }
        else if (rep[i]=='C')
        {
            syl.insertCharacterAtCursor('T');
            syl.insertCharacterAtCursor('S');					
        }
        else syl.insertCharacterAtCursor(rep[i]);
    }
    
    syl.normalize(finalTone);
    return syl;
}


const RomanizationSyllable FreeFormSyllable::convertToTLFromDT(unsigned int finalTone)
{
    string rep=internalForm();
    RomanizationSyllable syl;
    syl.setInputType(TLSyllable);
    
    unsigned int size=rep.length();
    for (unsigned int i=0; i<size; i++)	
    {
        char dt1 = rep[i];
        char lowdt1 = tolower(dt1);
        
        // r -> j (beginning)
        if (i==0 && lowdt1=='r') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('j', dt1));
            continue;
        }
        
        // replaces the two-character combinations
        if (i+1 < size) {
            string part=rep.substr(i, 2);
            string lower=toLowerString(part);
            
            // or -> o
            if (lower=="or") {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('o', part));
                i++;
                continue;
            }
            
            // en -> ian
            if (lower=="en") {                                          
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('i', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('a', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('n', part));
                i++;                                                    
                continue;
            }
            
            // et -> iat
            if (lower=="et") {                                          
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('i', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('a', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('t', part));
                i++;                                                    
                continue;
            }
            
            
            // bh -> b (beginning)
            if (i==0 && lower=="bh") {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('b', part));
                i++;
                continue;
            }
            
            // gh -> g (beginning)
            if (i==0 && lower=="gh") {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('g', part));
                i++;
                continue;
            }
            
            // wa -> ua (beginning)
            if (lower=="wa" && i==0) {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('u', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('a', part));
                i++;
                continue;
            }
            
            // we -> ue (beginning)
            if (lower=="we" && i==0) {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('u', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('e', part));
                i++;
                continue;
            }
            
            // wi -> ui (beginning)
            if (lower=="wi" && i==0) {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('u', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('i', part));
                i++;
                continue;
            }
            
            // yo -> io (beginning)
            if (lower=="yo" && i==0) {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('i', part));
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('o', part));
                i++;
                continue;
            }
            
            // yi -> i (beginning)
            if (lower=="yi" && i==0) {
                syl.insertCharacterAtCursor(charWithCaseAccordingTo('i', part));
                i++;
                continue;
            }
        }
        
        // o -> oo
        if (lowdt1=='o') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('o', dt1));
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('o', dt1));
            continue;
        }
        
        // b -> p (beginning)
        if (i==0 && lowdt1=='b') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('p', dt1));
            continue;
        }
        
        // p -> ph (beginning)
        if (i==0 && lowdt1=='p') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('p', dt1));
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('h', dt1));
            continue;
        }
        
        // k -> kh (beginning)
        if (i==0 && lowdt1=='k') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('k', dt1));
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('h', dt1));
            continue;
        }
        
        // g -> k (beginning)
        if (i==0 && lowdt1=='g') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('k', dt1));
            continue;
        }
        
        // d -> t (beginning)
        if (i==0 && lowdt1=='d') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('t', dt1));
            continue;
        }
        
        // t -> th (beginning)
        if (i==0 && lowdt1=='t') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('t', dt1));
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('h', dt1));
            continue;
        }
        
        // z -> ts (beginning)
        if (i==0 && lowdt1=='z') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('t', dt1));
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('s', dt1));
            continue;
        }
        
        // c -> tsh (beginning)
        if (i==0 && lowdt1=='c') {
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('t', dt1));
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('s', dt1));
            syl.insertCharacterAtCursor(charWithCaseAccordingTo('h', dt1));
            continue;
        }
        
        // else ...
        syl.insertCharacterAtCursor(dt1);
    }
    
    // remap the final tone
    unsigned int tltone=finalTone;
    
    syl.normalize(tltone);
    return syl;
    
}

char FreeFormSyllable::charWithCaseAccordingTo(char c, char ref) const
{
    if (tolower(ref) == ref) return tolower(c);
    return toupper(c);
}

char FreeFormSyllable::charWithCaseAccordingTo(char c, const string &r) const
{
    if (tolower(r[0]) == r[0]) return tolower(c);
    return toupper(c);
}

const string FreeFormSyllable::toLowerString(const string &s) const
{
    unsigned int size=s.length();
    string lower;
    unsigned int i;
    for (i=0;i<size;i++) lower+=string(1, tolower(s[i]));
    return lower;
}

