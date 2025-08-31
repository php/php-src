<?php

/** @generate-class-entries */

/** @not-serializable */
class IntlBreakIterator implements IteratorAggregate
{
    /** @cvalue BreakIterator::DONE */
    public const int DONE = UNKNOWN;

    /** @cvalue UBRK_WORD_NONE */
    public const int WORD_NONE = UNKNOWN;
    /** @cvalue UBRK_WORD_NONE_LIMIT */
    public const int WORD_NONE_LIMIT = UNKNOWN;
    /** @cvalue UBRK_WORD_NUMBER */
    public const int WORD_NUMBER = UNKNOWN;
    /** @cvalue UBRK_WORD_NUMBER_LIMIT */
    public const int WORD_NUMBER_LIMIT = UNKNOWN;
    /** @cvalue UBRK_WORD_LETTER */
    public const int WORD_LETTER = UNKNOWN;
    /** @cvalue UBRK_WORD_LETTER_LIMIT */
    public const int WORD_LETTER_LIMIT = UNKNOWN;
    /** @cvalue UBRK_WORD_KANA */
    public const int WORD_KANA = UNKNOWN;
    /** @cvalue UBRK_WORD_KANA_LIMIT */
    public const int WORD_KANA_LIMIT = UNKNOWN;
    /** @cvalue UBRK_WORD_IDEO */
    public const int WORD_IDEO = UNKNOWN;
    /** @cvalue UBRK_WORD_IDEO_LIMIT */
    public const int WORD_IDEO_LIMIT = UNKNOWN;

    /** @cvalue UBRK_LINE_SOFT */
    public const int LINE_SOFT = UNKNOWN;
    /** @cvalue UBRK_LINE_SOFT_LIMIT */
    public const int LINE_SOFT_LIMIT = UNKNOWN;
    /** @cvalue UBRK_LINE_HARD */
    public const int LINE_HARD = UNKNOWN;
    /** @cvalue UBRK_LINE_HARD_LIMIT */
    public const int LINE_HARD_LIMIT = UNKNOWN;

    /** @cvalue UBRK_SENTENCE_TERM */
    public const int SENTENCE_TERM = UNKNOWN;
    /** @cvalue UBRK_SENTENCE_TERM_LIMIT */
    public const int SENTENCE_TERM_LIMIT = UNKNOWN;
    /** @cvalue UBRK_SENTENCE_SEP */
    public const int SENTENCE_SEP = UNKNOWN;
    /** @cvalue UBRK_SENTENCE_SEP_LIMIT */
    public const int SENTENCE_SEP_LIMIT = UNKNOWN;

    /** @tentative-return-type */
    public static function createCharacterInstance(?string $locale = null): ?IntlBreakIterator {}

    /** @tentative-return-type */
    public static function createCodePointInstance(): IntlCodePointBreakIterator {}

    /** @tentative-return-type */
    public static function createLineInstance(?string $locale = null): ?IntlBreakIterator {}

    /** @tentative-return-type */
    public static function createSentenceInstance(?string $locale = null): ?IntlBreakIterator {}

    /** @tentative-return-type */
    public static function createTitleInstance(?string $locale = null): ?IntlBreakIterator {}

    /** @tentative-return-type */
    public static function createWordInstance(?string $locale = null): ?IntlBreakIterator {}

    private function __construct() {}

    /** @tentative-return-type */
    public function current(): int {}

    /** @tentative-return-type */
    public function first(): int {}

    /** @tentative-return-type */
    public function following(int $offset): int {}

    /** @tentative-return-type */
    public function getErrorCode(): int {}

    /** @tentative-return-type */
    public function getErrorMessage(): string {}

    /** @tentative-return-type */
    public function getLocale(int $type): string|false {}

    /** @tentative-return-type */
    public function getPartsIterator(string $type = IntlPartsIterator::KEY_SEQUENTIAL): IntlPartsIterator {}

    /** @tentative-return-type */
    public function getText(): ?string {}

    /** @tentative-return-type */
    public function isBoundary(int $offset): bool {}

    /** @tentative-return-type */
    public function last(): int {}

    /** @tentative-return-type */
    public function next(?int $offset = null): int {}

    /** @tentative-return-type */
    public function preceding(int $offset): int {}

    /** @tentative-return-type */
    public function previous(): int {}

    /** @tentative-return-type */
    public function setText(string $text): bool {}

    public function getIterator(): Iterator {}
}

/** @not-serializable */
class IntlRuleBasedBreakIterator extends IntlBreakIterator
{
    public function __construct(string $rules, bool $compiled = false) {}

    /** @tentative-return-type */
    public function getBinaryRules(): string|false {}

    /** @tentative-return-type */
    public function getRules(): string|false {}

    /** @tentative-return-type */
    public function getRuleStatus(): int {}

    /** @tentative-return-type */
    public function getRuleStatusVec(): array|false {}
}

/** @not-serializable */
class IntlCodePointBreakIterator extends IntlBreakIterator
{
    /** @tentative-return-type */
    public function getLastCodePoint(): int {}
}
