<?php

/** @file recursiveiteratoriterator.inc
 * @ingroup SPL
 * @brief class RecursiveIteratorIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Iterates through recursive iterators
 * @author  Marcus Boerger
 * @version 1.2
 * @since PHP 5.0
 *
 * The objects of this class are created by instances of RecursiveIterator.
 * Elements of those iterators may be traversable themselves. If so these
 * sub elements are recursed into.
 */
class RecursiveIteratorIterator implements OuterIterator
{
	/** Mode: Only show leaves */
	const LEAVES_ONLY 		= 0;
	/** Mode: Show parents prior to their children */
	const SELF_FIRST		= 1;
	/** Mode: Show all children prior to their parent */
	const CHILD_FIRST		= 2;

	/** Flag: Catches exceptions during getChildren() calls and simply jumps
	 * to the next element. */
	const CATCH_GET_CHILD	= 0x00000002;

	private $ait = array();
	private $count = 0;
	private $mode  = self::LEAVES_ONLY;
	private $flags = 0;

	/** Construct from RecursiveIterator
	 *
	 * @param it     RecursiveIterator to iterate
	 * @param mode   Operation mode (one of):
	 *               - LEAVES_ONLY only show leaves
	 *               - SELF_FIRST  show parents prior to their childs
	 *               - CHILD_FIRST show all children prior to their parent
	 * @param flags  Control flags, zero or any combination of the following
	 *               (since PHP 5.1).
	 *               - CATCH_GET_CHILD which catches exceptions during
	 *                 getChildren() calls and simply jumps to the next
	 *                 element.
	 */
	function __construct(RecursiveIterator $it, $mode = self::LEAVES_ONLY, $flags = 0)
	{
		$this->ait[0] = $it;
		$this->mode   = $mode;
		$this->flags  = $flags;
	}

	/** Rewind to top iterator as set in constructor
	 */
	function rewind()
	{
		while ($this->count) {
			unset($this->ait[$this->count--]);
			$this->endChildren();
		}
		$this->ait[0]->rewind();
		$this->ait[0]->recursed = false;
		callNextElement(true);
	}

	/** @return whether iterator is valid
	 */
	function valid()
	{
		$count = $this->count;
		while ($count) {
			$it = $this->ait[$count];
			if ($it->valid()) {
				return true;
			}
			$count--;
			$this->endChildren();
		}
		return false;
	}

	/** @return current key
	 */
	function key()
	{
		$it = $this->ait[$this->count];
		return $it->key();
	}

	/** @return current element
	 */
	function current()
	{
		$it = $this->ait[$this->count];
		return $it->current();
	}

	/** Forward to next element
	 */
	function next()
	{
		while ($this->count) {
			$it = $this->ait[$this->count];
			if ($it->valid()) {
				if (!$it->recursed && callHasChildren()) {
					$it->recursed = true;
					try
					{
						$sub = callGetChildren();
					}
					catch (Exception $e)
					{
						if (!($this->flags & self::CATCH_GET_CHILD))
						{
							throw $e;
						}
						$it->next();
						continue;
					}
					$sub->recursed = false;
					$sub->rewind();
					if ($sub->valid()) {
						$this->ait[++$this->count] = $sub;
						if (!$sub instanceof RecursiveIterator) {
							throw new Exception(get_class($sub).'::getChildren() must return an object that implements RecursiveIterator');
						}
						$this->beginChildren();
						return;
					}
					unset($sub);
				}
				$it->next();
				$it->recursed = false;
				if ($it->valid()) {
					return;
				}
				$it->recursed = false;
			}
			if ($this->count) {
				unset($this->ait[$this->count--]);
				$it = $this->ait[$this->count];
				$this->endChildren();
				callNextElement(false);
			}
		}
		callNextElement(true);
	}

	/** @return Sub Iterator at given level or if unspecified the current sub
	 *          Iterator
	 */
	function getSubIterator($level = NULL)
	{
		if (is_null($level)) {
			$level = $this->count;
		}
		return @$this->ait[$level];
	}

	/**
	 * @return The inner iterator
	 */
	function getInnerIterator()
	{
		return $this->it;
	}

	/** @return Current Depth (Number of parents)
	 */
	function getDepth()
	{
		return $this->level;
	}

	/** @return whether current sub iterators current element has children
	 * @since PHP 5.1
	 */
	function callHasChildren()
	{
		return $this->ait[$this->count]->hasChildren();
	}

	/** @return current sub iterators current children
	 * @since PHP 5.1
	 */
	function callGetChildren()
	{
		return $this->ait[$this->count]->getChildren();
	}

	/** Called right after calling getChildren() and its rewind().
	 * @since PHP 5.1
	 */
	function beginChildren()
	{
	}

	/** Called after current child iterator is invalid and right before it
	 * gets destructed.
	 * @since PHP 5.1
	 */
	function endChildren()
	{
	}

	private function callNextElement($after_move)
	{
		if ($this->valid())
		{
			if ($after_move)
			{
				if (($this->mode == self::SELF_FIRST && $this->callHasChildren())
				||   $this->mode == self::LEAVES_ONLY)
				$this->nextElement();
			}
			else
			{
				$this->nextElement();
			}
		}
	}

	/** Called when the next element is available
	 */
	function nextElement()
	{
	}
}

?>
