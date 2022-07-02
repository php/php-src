<?php

/** @file recursivedualiterator.inc
 * @ingroup Examples
 * @brief class RecursiveDualIterator
 * @author  Marcus Boerger
 * @date    2003 - 2006
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   Synchronous iteration over two recursive iterators
 * @author  Marcus Boerger
 * @version 1.0
 */
class RecursiveDualIterator extends DualIterator implements RecursiveIterator
{
    private $ref;

    /** construct iterator from two RecursiveIterator instances
     *
     * @param lhs   Left  Hand Side Iterator
     * @param rhs   Right Hand Side Iterator
     * @param flags iteration flags
     */
    function __construct(RecursiveIterator $lhs, RecursiveIterator $rhs,
                $flags = 0x33 /*DualIterator::DEFAULT_FLAGS*/)
    {
        parent::__construct($lhs, $rhs, $flags);
    }

    /** @return bool whether both LHS and RHS have children
     */
    function hasChildren(): bool
    {
        return $this->getLHS()->hasChildren() && $this->getRHS()->hasChildren();
    }

    /** @return RecursiveDualIterator (late binding) for the two inner
     * iterators current children.
     */
    function getChildren(): RecursiveDualIterator
    {
        if (empty($this->ref))
        {
            $this->ref = new ReflectionClass($this);
        }
        return $this->ref->newInstance(
                    $this->getLHS()->getChildren(), $this->getRHS()->getChildren(), $this->getFlags());
    }

    /** @return bool whether both inner iterators are valid, have same hasChildren()
     * state and identical current and key values or both are non valid.
     */
    function areIdentical(): bool
    {
        return $this->getLHS()->hasChildren() === $this->getRHS()->hasChildren()
            && parent::areIdentical();
    }

    /** @return bool whether both inner iterators are valid, have same hasChildren()
     * state and equal current and key values or both are invalid.
     */
    function areEqual(): bool
    {
        return $this->getLHS()->hasChildren() === $this->getRHS()->hasChildren()
            && parent::areEqual();
    }
}

?>
