/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

#include "codepointiterator_internal.h"
#include <unicode/uchriter.h>
#include <typeinfo>

//copied from cmemory.h, which is not public
typedef union {
    long    t1;
    double  t2;
    void   *t3;
} UAlignedMemory;

#define U_POINTER_MASK_LSB(ptr, mask) (((ptrdiff_t)(char *)(ptr)) & (mask))
#define U_ALIGNMENT_OFFSET(ptr) U_POINTER_MASK_LSB(ptr, sizeof(UAlignedMemory) - 1)
#define U_ALIGNMENT_OFFSET_UP(ptr) (sizeof(UAlignedMemory) - U_ALIGNMENT_OFFSET(ptr))

using namespace PHP;

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CodePointBreakIterator);

CodePointBreakIterator::CodePointBreakIterator()
: BreakIterator(), fCharIter(NULL), lastCodePoint(U_SENTINEL)
{
	UErrorCode uec = UErrorCode();
	this->fText = utext_openUChars(NULL, NULL, 0, &uec);
}

CodePointBreakIterator::CodePointBreakIterator(const PHP::CodePointBreakIterator &other)
: BreakIterator(other), fText(NULL), fCharIter(NULL), lastCodePoint(U_SENTINEL)
{
	*this = other;
}

CodePointBreakIterator& CodePointBreakIterator::operator=(const CodePointBreakIterator& that)
{
	UErrorCode uec = UErrorCode();
	UText *ut_clone = NULL;

	if (this == &that) {
		return *this;
	}

	this->fText = utext_clone(this->fText, that.fText, FALSE, TRUE, &uec);

	//don't bother copying the character iterator, getText() is deprecated
	clearCurrentCharIter();

	this->lastCodePoint = that.lastCodePoint;
	return *this;
}

CodePointBreakIterator::~CodePointBreakIterator()
{
	if (this->fText) {
		utext_close(this->fText);
	}
	clearCurrentCharIter();
}

UBool CodePointBreakIterator::operator==(const BreakIterator& that) const
{
	if (typeid(*this) != typeid(that)) {
		return FALSE;
	}

	const CodePointBreakIterator& that2 =
		static_cast<const CodePointBreakIterator&>(that);

	if (!utext_equals(this->fText, that2.fText)) {
		return FALSE;
	}

	return TRUE;
}

CodePointBreakIterator* CodePointBreakIterator::clone(void) const
{
	return new CodePointBreakIterator(*this);
}

CharacterIterator& CodePointBreakIterator::getText(void) const
{
	if (this->fCharIter == NULL) {
		//this method is deprecated anyway; setup bogus iterator
		static const UChar c = 0;
		this->fCharIter = new UCharCharacterIterator(&c, 0);
	}

	return *this->fCharIter;
}

UText *CodePointBreakIterator::getUText(UText *fillIn, UErrorCode &status) const
{
	return utext_clone(fillIn, this->fText, FALSE, TRUE, &status);
}

void CodePointBreakIterator::setText(const UnicodeString &text)
{
	UErrorCode uec = UErrorCode();

	//this closes the previous utext, if any
	this->fText = utext_openConstUnicodeString(this->fText, &text, &uec);

	clearCurrentCharIter();
}

void CodePointBreakIterator::setText(UText *text, UErrorCode &status)
{
	if (U_FAILURE(status)) {
		return;
	}

	this->fText = utext_clone(this->fText, text, FALSE, TRUE, &status);

	clearCurrentCharIter();
}

void CodePointBreakIterator::adoptText(CharacterIterator* it)
{
	UErrorCode uec = UErrorCode();
	clearCurrentCharIter();

	this->fCharIter = it;
	this->fText = utext_openCharacterIterator(this->fText, it, &uec);
}

int32_t CodePointBreakIterator::first(void)
{
	UTEXT_SETNATIVEINDEX(this->fText, 0);
	this->lastCodePoint = U_SENTINEL;

	return 0;
}

int32_t CodePointBreakIterator::last(void)
{
	int32_t pos = (int32_t)utext_nativeLength(this->fText);
	UTEXT_SETNATIVEINDEX(this->fText, pos);
	this->lastCodePoint = U_SENTINEL;

	return pos;
}

int32_t CodePointBreakIterator::previous(void)
{
	this->lastCodePoint = UTEXT_PREVIOUS32(this->fText);
	if (this->lastCodePoint == U_SENTINEL) {
		return BreakIterator::DONE;
	}

	return (int32_t)UTEXT_GETNATIVEINDEX(this->fText);
}

int32_t CodePointBreakIterator::next(void)
{
	this->lastCodePoint = UTEXT_NEXT32(this->fText);
	if (this->lastCodePoint == U_SENTINEL) {
		return BreakIterator::DONE;
	}

	return (int32_t)UTEXT_GETNATIVEINDEX(this->fText);
}

int32_t CodePointBreakIterator::current(void) const
{
	return (int32_t)UTEXT_GETNATIVEINDEX(this->fText);
}

int32_t CodePointBreakIterator::following(int32_t offset)
{
	this->lastCodePoint = utext_next32From(this->fText, offset);
	if (this->lastCodePoint == U_SENTINEL) {
		return BreakIterator::DONE;
	}

	return (int32_t)UTEXT_GETNATIVEINDEX(this->fText);
}

int32_t CodePointBreakIterator::preceding(int32_t offset)
{
	this->lastCodePoint = utext_previous32From(this->fText, offset);
	if (this->lastCodePoint == U_SENTINEL) {
		return BreakIterator::DONE;
	}

	return (int32_t)UTEXT_GETNATIVEINDEX(this->fText);
}

UBool CodePointBreakIterator::isBoundary(int32_t offset)
{
	//this function has side effects, and it's supposed to
	utext_setNativeIndex(this->fText, offset);
	return (offset == utext_getNativeIndex(this->fText));
}

int32_t CodePointBreakIterator::next(int32_t n)
{
	UBool res = utext_moveIndex32(this->fText, n);

#ifndef UTEXT_CURRENT32
#define UTEXT_CURRENT32 utext_current32
#endif

	if (res) {
		this->lastCodePoint = UTEXT_CURRENT32(this->fText);
		return (int32_t)UTEXT_GETNATIVEINDEX(this->fText);
	} else {
		this->lastCodePoint = U_SENTINEL;
		return BreakIterator::DONE;
	}
}

CodePointBreakIterator *CodePointBreakIterator::createBufferClone(
	void *stackBuffer, int32_t &bufferSize, UErrorCode &status)
{
	//see implementation of RuleBasedBreakIterator::createBufferClone()
	if (U_FAILURE(status)) {
		return NULL;
	}

	if (bufferSize <= 0) {
		bufferSize = sizeof(CodePointBreakIterator) + U_ALIGNMENT_OFFSET_UP(0);
		return NULL;
	}

	char *buf = (char*)stackBuffer;
	uint32_t s = bufferSize;

	if (stackBuffer == NULL) {
		 s = 0;
	}

	if (U_ALIGNMENT_OFFSET(stackBuffer) != 0) {
		uint32_t offsetUp = (uint32_t)U_ALIGNMENT_OFFSET_UP(buf);
		s -= offsetUp;
		buf += offsetUp;
	}

	if (s < sizeof(CodePointBreakIterator)) {
		CodePointBreakIterator *clonedBI = new CodePointBreakIterator(*this);
		if (clonedBI == NULL) {
			status = U_MEMORY_ALLOCATION_ERROR;
		} else {
			status = U_SAFECLONE_ALLOCATED_WARNING;
		}

		return clonedBI;
	}

	return new(buf) CodePointBreakIterator(*this);
}

CodePointBreakIterator &CodePointBreakIterator::refreshInputText(UText *input, UErrorCode &status)
{
	//see implementation of RuleBasedBreakIterator::createBufferClone()
	if (U_FAILURE(status)) {
		return *this;
	}
	if (input == NULL) {
		status = U_ILLEGAL_ARGUMENT_ERROR;
		return *this;
	}

	int64_t pos = utext_getNativeIndex(this->fText);
	this->fText = utext_clone(this->fText, input, FALSE, TRUE, &status);
	if (U_FAILURE(status)) {
		return *this;
	}

	utext_setNativeIndex(this->fText, pos);
	if (utext_getNativeIndex(fText) != pos) {
		status = U_ILLEGAL_ARGUMENT_ERROR;
	}

	return *this;
}
