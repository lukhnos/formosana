//
// TaiwaneseRomanization.h
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

#ifndef TaiwaneseRomanization_h
#define TaiwaneseRomanization_h

#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>
#include "VowelHelper.h"

namespace Formosa {
    namespace TaiwaneseRomanization {

        using namespace std;
    
        class Composable {
        public:
            virtual ~Composable();
            virtual void clear() = 0;
            virtual bool empty() const = 0;
            virtual unsigned int numberOfCodepoints() const = 0;
            virtual const string composedForm() = 0;
        }; 

        class ComposableStringBuffer : Composable {
        public:
            ComposableStringBuffer();
            virtual unsigned int cursor();
            virtual unsigned int setCursor(unsigned int c);
            virtual void clear();
            virtual bool empty();
            virtual unsigned int numberOfCodepoints();
            virtual bool insertCharacterAt(unsigned int i, char c);
            virtual bool removeCodepointAt(unsigned int i);
            virtual const string composedForm();
            virtual const string internalForm();
        
        protected:
            vector<string> strvec;
            unsigned int _cursor;
        };
    
    
        enum SyllableType {
            POJSyllable = 0,
            TLSyllable = 1,
            TLPASyllable = 2,
            DTSyllable = 3,            
            HakkaPFSSyllable = 4
        };
    
        enum DiacriticInputOption {
            DiacriticGivenBeforeVowel = 0,
            DiacriticGivenAfterVowel = 1
        };

    	class RomanizationSymbol {
    	public:
    		RomanizationSymbol();
    		RomanizationSymbol(const string &s, SyllableType t);
    		RomanizationSymbol(const RomanizationSymbol &s);
    		void setType(SyllableType t);
    		const RomanizationSymbol& operator=(const RomanizationSymbol &s);
    		const string symbol() const;
    		const string symbolInLowerCase() const;
    		const string setSymbol(const string& s);
    		const string composedForm(bool forcePOJStyle = false, bool usePOJLegacyOu = false) const;
    		unsigned int composedLength() const;
    		unsigned int tone() const;
    		unsigned int setTone(unsigned int t);
    		bool isUpperCase() const;
		
    	protected:
    		unsigned int _tone;
    		SyllableType _type;
    		string _symbol;
    	};


    	// the _cursor in RomanizationSyllable really means "the internal symbol cursor",
    	// as for the display cursor (called by the input method context), 
    	// we need to recalculate
    	class RomanizationSyllable : public Composable
        {
        public:
    		RomanizationSyllable();
    		RomanizationSyllable(const RomanizationSyllable &s);
    		const RomanizationSyllable& operator=(const RomanizationSyllable &s);
    		virtual void setInputType(SyllableType t);
    		virtual void setInputOption(DiacriticInputOption o);
    		virtual void setForcePOJStyle(bool p);
    		virtual void setUsePOJLegacyOU(bool u);
            virtual void clear();
            virtual bool empty() const;
            virtual unsigned int numberOfCodepoints() const;
            virtual const string composedForm();
    		void setCursor(unsigned int c);
    		unsigned int cursor() const;
    		bool cursorHome();
    		bool cursorEnd();
    		bool cursorLeft();
    		bool cursorRight();
    		bool insertSymbolAtCursor(const RomanizationSymbol &s);

    		// if there is a prepared tone, the given tone parameter will be ignored
    		bool insertCharacterAtCursor(char c, unsigned int tone = 0);
		
            // backspace
    		bool removeCharacterAtRightOfCursor();
		
            // delete
    		bool removeCharacterAtLeftOfCursor();
	
    		// returns a normalized string that represents the "internal form" for querying the database
    		// implies normalization
    		const string normalizedQueryData(unsigned int finalTone = 0);
	
    		// normalization is an "identpotent" function, ie. the result should
    		// be the same no matter how many times you call it--this being a very
    		// important linguistic characteristic of this function	
    		void normalize(unsigned int finalTone = 0);
        		
    		RomanizationSyllable convertToPOJSyllable();
    		RomanizationSyllable convertToTLSyllable();
		
		
    	protected:
            static char charAccordingToCaseOf(char c, char ref);
            
    		bool atBeginning() const;
    		bool atEnd() const;
    		void clearPreparedTone();
    		bool hasPreviousSymbolAtCursor() const;
    		bool hasNextSymbol(unsigned int pos) const;
    		bool hasNextNextSymbol(unsigned int pos) const;

		
    		// the result of this function is unpredictable if there's no
    		// previous symbol--always check with hasPreviousSymbolAtCursor() !
    		RomanizationSymbol& previousSymbolAtCursor();

    		// always assumes that the given input is in all lower case
    		unsigned int findSymbol(const char *s) const;
    		unsigned int findSymbolPair(const char *s1, const char *s2) const;
    		unsigned int findSymbolTriple(const char *s1, const char *s2, const char *s3) const;
		
    		SyllableType _inputType;
    		DiacriticInputOption _inputOption;
    		bool _forcePOJStyle;
    		bool _usePOJLegacyOU;
		        
    		vector<RomanizationSymbol> _symvec;
    		unsigned int _cursor;
    		unsigned int _preparedTone;
    	};
	
    	class FreeFormSyllable : public ComposableStringBuffer
    	{
    	public:
    		const RomanizationSyllable convertToTLFromTLPA(unsigned int finalTone = 0);
    		const RomanizationSyllable convertToTLFromDT(unsigned int finalTone = 0);

    	protected:
    		char charWithCaseAccordingTo(char c, char ref) const;
    		char charWithCaseAccordingTo(char c, const string &r) const;
    		const string toLowerString(const string &s) const;
    	};
    };
};

#endif
