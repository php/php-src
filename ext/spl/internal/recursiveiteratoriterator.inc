<?php

/** @file recursiveiteratoriterator.inc
 * @ingroup SPL
 * @brief class RecursiveIteratorIterator
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

define('RIT_LEAVES_ONLY', 0);
define('RIT_SELF_FIRST',  1);
define('RIT_CHILD_FIRST', 2);
define('RIT_CATCH_GET_CHILD', 256);

/**
 * @brief   Iterates through recursive iterators
 * @author  Marcus Boerger
 * @version 1.1
 * @since PHP 5.0
 *
 * The objects of this class are created by instances of RecursiveIterator. 
 * Elements of those iterators may be traversable themselves. If so these 
 * sub elements are recursed into.
 */
class RecursiveIteratorIterator implements OuterIterator
{
	private $ait = array();
	private $count = 0;
	private $mode  = RIT_LEAVES_ONLY;
	private $flags = 0;

	/** Construct from RecursiveIterator
	 *
	 * @param it     RecursiveIterator to iterate
	 * @param flags  Operation mode (one of):
	 *               - RIT_LEAVES_ONLY only show leaves
	 *               - RIT_SELF_FIRST  show parents prior to their childs
	 *               - RIT_CHILD_FIRST show all childs prior to their parent
	 *               or'ed with the following flags:
	 *               - RIT_CATCH_GET_CHILD which catches exceptions during
	 *                 getChildren() calls and simply jumps to the next 
	 *                 element.
	 */
	function __construct(RecursiveIterator $it, $flags)
	{
		$this->ait[0] = $it;
		$this->mode   = $flags & 0xFF;
		$this->flags  = $flags & ~0xFF;
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
						if (!($this->flags & RIT_CATCH_GET_CHILD))
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
			}
		}
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
}

?>