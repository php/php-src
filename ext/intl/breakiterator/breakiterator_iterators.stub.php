<?php

/** @generate-class-entries */

/** @not-serializable */
class IntlPartsIterator extends IntlIterator
{
    /**
     * @var int
     * @cvalue PARTS_ITERATOR_KEY_SEQUENTIAL
     */
    public const KEY_SEQUENTIAL = UNKNOWN;
    /**
     * @var int
     * @cvalue PARTS_ITERATOR_KEY_LEFT
     */
    public const KEY_LEFT = UNKNOWN;
    /**
     * @var int
     * @cvalue PARTS_ITERATOR_KEY_RIGHT
     */
    public const KEY_RIGHT = UNKNOWN;

    /** @tentative-return-type */
    public function getBreakIterator(): IntlBreakIterator {}

    /** @tentative-return-type */
    public function getRuleStatus(): int {}
}
