/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#ifndef CODEPOINTITERATOR_INTERNAL_H
#define CODEPOINTITERATOR_INTERNAL_H

#include <unicode/brkiter.h>
#include <unicode/unistr.h>

using icu::BreakIterator;
using icu::CharacterIterator;
using icu::UnicodeString;

namespace PHP {

	class CodePointBreakIterator : public BreakIterator {

	public:
		static UClassID getStaticClassID();

		CodePointBreakIterator();

		CodePointBreakIterator(const CodePointBreakIterator &other);

		CodePointBreakIterator& operator=(const CodePointBreakIterator& that);

		virtual ~CodePointBreakIterator();

		virtual UBool operator==(const BreakIterator& that) const;

		virtual CodePointBreakIterator* clone(void) const;

		virtual UClassID getDynamicClassID(void) const;

		virtual CharacterIterator& getText(void) const;

		virtual UText *getUText(UText *fillIn, UErrorCode &status) const;

		virtual void setText(const UnicodeString &text);

		virtual void setText(UText *text, UErrorCode &status);

		virtual void adoptText(CharacterIterator* it);

		virtual int32_t first(void);

		virtual int32_t last(void);

		virtual int32_t previous(void);

		virtual int32_t next(void);

		virtual int32_t current(void) const;

		virtual int32_t following(int32_t offset);

		virtual int32_t preceding(int32_t offset);

		virtual UBool isBoundary(int32_t offset);

		virtual int32_t next(int32_t n);

		virtual CodePointBreakIterator *createBufferClone(void *stackBuffer,
														  int32_t &BufferSize,
														  UErrorCode &status);

		virtual CodePointBreakIterator &refreshInputText(UText *input, UErrorCode &status);

		inline UChar32 getLastCodePoint()
		{
			return this->lastCodePoint;
		}

	private:
		UText *fText;
		UChar32 lastCodePoint;
		mutable CharacterIterator *fCharIter;

		inline void clearCurrentCharIter()
		{
			delete this->fCharIter;
			this->fCharIter = NULL;
			this->lastCodePoint = U_SENTINEL;
		}
	};
}

#endif
