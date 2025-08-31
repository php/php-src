<?php

/** @file recursivecomparedualiterator.inc
 * @ingroup Examples
 * @brief class DualIterator
 * @author  Marcus Boerger
 * @date    2003 - 2006
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   Recursive comparison iterator for a RecursiveDualIterator
 * @author  Marcus Boerger
 * @version 1.0
 */
class RecursiveCompareDualIterator extends RecursiveIteratorIterator
{
    /** Used to keep end of recursion equality. That is en leaving a nesting
     * level we need to check whether both child iterators are at their end.
     */
    protected $equal = false;

    /** Construct from RecursiveDualIterator
     *
     * @param $it      RecursiveDualIterator
     * @param $mode    should be LEAVES_ONLY
     * @param $flags   should be 0
     */
    function __construct(RecursiveDualIterator $it, $mode = self::LEAVES_ONLY, $flags = 0)
    {
        parent::__construct($it);
    }

    /** Rewind iteration and comparison process. Starting with $equal = true.
     */
    function rewind(): void
    {
        $this->equal = true;
        parent::rewind();
    }

    /** Calculate $equal
     * @see $equal
     */
    function endChildren(): void
    {
        $this->equal &= !$this->getInnerIterator()->getLHS()->valid()
                     && !$this->getInnerIterator()->getRHS()->valid();
    }

    /** @return whether both inner iterators are valid and have identical
     * current and key values or both are non valid.
     */
    function areIdentical()
    {
        return $this->equal && $this->getInnerIterator()->areIdentical();
    }

    /** @return whether both inner iterators are valid and have equal current
     * and key values or both are non valid.
     */
    function areEqual()
    {
        return $this->equal && $this->getInnerIterator()->areEqual();
    }
}

?>
