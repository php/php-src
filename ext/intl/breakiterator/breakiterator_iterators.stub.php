<?php

/** @generate-class-entries */

/** @not-serializable */
class IntlPartsIterator extends IntlIterator
{
    /**
     * @cvalue PARTS_ITERATOR_KEY_SEQUENTIAL
     */
    public const int KEY_SEQUENTIAL = UNKNOWN;
    /**
     * @cvalue PARTS_ITERATOR_KEY_LEFT
     */
    public const int KEY_LEFT = UNKNOWN;
    /**
     * @cvalue PARTS_ITERATOR_KEY_RIGHT
     */
    public const int KEY_RIGHT = UNKNOWN;

    /** @tentative-return-type */
    public function getBreakIterator(): IntlBreakIterator {}

    /** @tentative-return-type */
    public function getRuleStatus(): int {}
}
