/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

		~CodePointBreakIterator() override;

#if U_ICU_VERSION_MAJOR_NUM >= 70
		bool operator==(const BreakIterator& that) const override;
#else
		UBool operator==(const BreakIterator& that) const override;
#endif

		CodePointBreakIterator* clone(void) const override;

		UClassID getDynamicClassID(void) const override;

		CharacterIterator& getText(void) const override;

		UText *getUText(UText *fillIn, UErrorCode &status) const override;

		void setText(const UnicodeString &text) override;

		void setText(UText *text, UErrorCode &status) override;

		void adoptText(CharacterIterator* it) override;

		int32_t first(void) override;

		int32_t last(void) override;

		int32_t previous(void) override;

		int32_t next(void) override;

		int32_t current(void) const override;

		int32_t following(int32_t offset) override;

		int32_t preceding(int32_t offset) override;

		UBool isBoundary(int32_t offset) override;

		int32_t next(int32_t n) override;

		CodePointBreakIterator *createBufferClone(void *stackBuffer,
														  int32_t &BufferSize,
														  UErrorCode &status) override;

		CodePointBreakIterator &refreshInputText(UText *input, UErrorCode &status) override;

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
