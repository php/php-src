<?php

/** @generate-class-entries */

class IntlBreakIterator implements IteratorAggregate
{
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
    public function setText(string $text): ?bool {} // TODO return false instead of null in case of failure

    public function getIterator(): Iterator {}
}

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

class IntlCodePointBreakIterator extends IntlBreakIterator
{
    /** @tentative-return-type */
    public function getLastCodePoint(): int {}
}
