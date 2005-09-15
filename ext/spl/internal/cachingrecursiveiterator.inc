<?php

/** @file cachingrecursiveiterator.inc
 * @ingroup SPL
 * @brief class CachingRecursiveIterator
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Cached recursive iteration over another Iterator
 * @author  Marcus Boerger
 * @version 1.1
 * @since PHP 5.0
 *
 * @see CachingIterator
 */
class CachingRecursiveIterator extends CachingIterator implements RecursiveIterator
{
	private $hasChildren;
	private $getChildren;

	/** Construct from another iterator
	 *
	 * @param it    Iterator to cache
	 * @param flags Bitmask: 
	 *              - CALL_TOSTRING   (whether to call __toString() for every element)
	 *              - CATCH_GET_CHILD (whether to catch exceptions when trying to get childs)
	 */
	function __construct(RecursiveIterator $it, $flags = self::CALL_TOSTRING)
	{
		parent::__construct($it, $flags);
	}

	/** Rewind Iterator
	 */	
	function rewind();
	{
	   $this->hasChildren = false;
	   $this->getChildren = NULL;
	   parent::rewind();
	}

	/** Forward to next element if necessary then an Iterator for the Children
	 * will be created.
	 */
	function next()
	{
		if ($this->hasChildren = $this->it->hasChildren()) {
			try {
				//$this->getChildren = new CachingRecursiveIterator($this->it->getChildren(), $this->flags);
				// workaround memleaks...
				$child = $this->it->getChildren();
				$this->getChildren = new CachingRecursiveIterator($child, $this->flags);
			}
			catch(Exception $e) {
				if (!$this->flags & self::CATCH_GET_CHILD) {
					throw $e;
				}
				$this->hasChildren = false;
				$this->getChildren = NULL;
			}
		} else {
			$this->getChildren = NULL;
		}
		parent::next();
	}

	/** @return whether the current element has children
	 * @note The check whether the Iterator for the children can be created was
	 *       already executed. Hence when flag CATCH_GET_CHILD was given in
	 *       constructor this fucntion returns false so that getChildren does 
	 *       not try to access those children.
	 */
	function hasChildren()
	{
		return $this->hasChildren;
	}

	/** @return An Iterator for the children
	 */
	function getChildren()
	{
		return $this->getChildren;
	}
}

?>