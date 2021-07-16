<?php

/** @file dualiterator.inc
 * @ingroup Examples
 * @brief class DualIterator
 * @author  Marcus Boerger
 * @date    2003 - 2006
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   Synchronous iteration over two iterators
 * @author  Marcus Boerger
 * @version 1.3
 */
class DualIterator implements Iterator
{
    const CURRENT_LHS   = 0x01;
    const CURRENT_RHS   = 0x02;
    const CURRENT_ARRAY = 0x03;
    const CURRENT_0     = 0x00;

    const KEY_LHS   = 0x10;
    const KEY_RHS   = 0x20;
    const KEY_0     = 0x00;

    const DEFAULT_FLAGS = 0x13;

    private $lhs;
    private $rhs;
    private $flags;

    /** construct iterator from two iterators
     *
     * @param lhs   Left  Hand Side Iterator
     * @param rhs   Right Hand Side Iterator
     * @param flags iteration flags
     */
    function __construct(Iterator $lhs, Iterator $rhs,
                    $flags = 0x13 /*DualIterator::DEFAULT_FLAGS*/)
    {
        $this->lhs   = $lhs;
        $this->rhs   = $rhs;
        $this->flags = $flags;
    }

    /** @return Left Hand Side Iterator
     */
    function getLHS()
    {
        return $this->lhs;
    }

    /** @return Right Hand Side Iterator
     */
    function getRHS()
    {
        return $this->rhs;
    }

    /** @param flags new flags
     */
    function setFlags($flags)
    {
        $this->flags = $flags;
    }

    /** @return current flags
     */
    function getFlags()
    {
        return $this->flags;
    }

    /** rewind both inner iterators
     */
    function rewind(): void
    {
        $this->lhs->rewind();
        $this->rhs->rewind();
    }

    /** @return whether both inner iterators are valid
     */
    function valid(): bool
    {
        return $this->lhs->valid() && $this->rhs->valid();
    }

    /** @return current value depending on CURRENT_* flags
     */
    function current(): mixed
    {
        switch($this->flags & 0x0F)
        {
        default:
        case self::CURRENT_ARRAY:
            return array($this->lhs->current(), $this->rhs->current());
        case self::CURRENT_LHS:
            return $this->lhs->current();
        case self::CURRENT_RHS:
            return $this->rhs->current();
        case self::CURRENT_0:
            return NULL;
        }
    }

    /** @return key value depending on KEY_* flags
     */
    function key(): mixed
    {
        switch($this->flags & 0xF0)
        {
        default:
        case self::KEY_LHS:
            return $this->lhs->key();
        case self::KEY_RHS:
            return $this->rhs->key();
        case self::KEY_0:
            return NULL;
        }
    }

    /** move both inner iterators forward
     */
    function next(): void
    {
        $this->lhs->next();
        $this->rhs->next();
    }

    /** @return whether both inner iterators are valid and have identical
     * current and key values or both are non valid.
     */
    function areIdentical()
    {
        return $this->valid()
             ? $this->lhs->current() === $this->rhs->current()
            && $this->lhs->key()     === $this->rhs->key()
             : $this->lhs->valid()   ==  $this->rhs->valid();
    }

    /** @return whether both inner iterators are valid and have equal current
     * and key values or both are non valid.
     */
    function areEqual()
    {
        return $this->valid()
             ? $this->lhs->current() ==  $this->rhs->current()
            && $this->lhs->key()     ==  $this->rhs->key()
             : $this->lhs->valid()   ==  $this->rhs->valid();
    }

    /** Compare two iterators
     *
     * @param lhs   Left  Hand Side Iterator
     * @param rhs   Right Hand Side Iterator
     * @param identical whether to use areEqual() or areIdentical()
     * @return whether both iterators are equal/identical
     *
     * @note If one implements RecursiveIterator the other must do as well.
     *       And if both do then a recursive comparison is being used.
     */
    static function compareIterators(Iterator $lhs, Iterator $rhs,
                                     $identical = false)
    {
        if ($lhs instanceof RecursiveIterator)
        {
            if ($rhs instanceof RecursiveIterator)
            {
                $it = new RecursiveDualIterator($lhs, $rhs,
                                self::CURRENT_0 | self::KEY_0);
                $it = new RecursiveCompareDualIterator($it);
            }
            else
            {
                return false;
            }
        }
        else
        {
            $it = new DualIterator($lhs, $rhs, self::CURRENT_0 | self::KEY_0);
        }

        if ($identical)
        {
            foreach($it as $n)
            {
                if (!$it->areIdentical())
                {
                    return false;
                }
            }
        }
        else
        {
            foreach($it as $n)
            {
                if (!$it->areEqual())
                {
                    return false;
                }
            }
        }
        return $identical ? $it->areIdentical() : $it->areEqual();
    }
}

?>
