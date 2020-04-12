<?php

/** @generate-function-entries */

class IntlBreakIterator implements Traversable
{
    /** @return IntlBreakIterator|null */
    public static function createCharacterInstance(?string $locale = null) {}

    /** @return IntlCodePointBreakIterator */
    public static function createCodePointInstance() {}

    /** @return IntlBreakIterator|null */
    public static function createLineInstance(?string $locale = null) {}

    /** @return IntlBreakIterator|null */
    public static function createSentenceInstance(?string $locale = null) {}

    /** @return IntlBreakIterator|null */
    public static function createTitleInstance(?string $locale = null) {}

    /** @return IntlBreakIterator|null */
    public static function createWordInstance(?string $locale = null) {}

    private function __construct() {}

    /** @return int */
    public function current() {}

    /** @return int */
    public function first() {}

    /** @return int|false */
    public function following(int $offset) {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string|false */
    public function getErrorMessage() {}

    /** @return string */
    public function getLocale(int $locale_type) {}

    /** @return IntlPartsIterator|false */
    public function getPartsIterator(string $key_type = IntlPartsIterator::KEY_SEQUENTIAL) {}

    /** @return string|null */
    public function getText() {}

    /** @return bool */
    public function isBoundary(int $offset) {}

    /** @return int */
    public function last() {}

    /** @return int|false */
    public function next(?int $offset = null) {}

    /** @return int|false */
    public function preceding(int $offset) {}

    /** @return int */
    public function previous() {}

    /** @return bool|null */
    public function setText(string $text) {}
}

class IntlRuleBasedBreakIterator extends IntlBreakIterator
{
    public function __construct(string $rules, bool $areCompiled = false) {}

    /** @return string|false */
    public function getBinaryRules() {}

    /** @return string|false */
    public function getRules() {}

    /** @return int */
    public function getRuleStatus() {}

    /** @return array|false */
    public function getRuleStatusVec() {}
}

class IntlPartsIterator extends IntlIterator
{
    /** @return IntlBreakIterator */
    public function getBreakIterator() {}
}

class IntlCodePointBreakIterator extends IntlBreakIterator
{
    /** @return int */
    public function getLastCodePoint() {}
}
