<?php

/** @file recursivecachingiterator.inc
 * @ingroup SPL
 * @brief class RecursiveCachingIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Cached recursive iteration over another Iterator
 * @author  Marcus Boerger
 * @version 1.2
 * @since PHP 5.1
 *
 * @see CachingIterator
 */
class RecursiveCachingIterator extends CachingIterator implements RecursiveIterator
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
		if ($this->hasChildren = $this->it->hasChildren())
		{
			try
			{
				$child = $this->it->getChildren();
				if (!$this->ref)
				{
					$this->ref = new ReflectionClass($this);
				}
				$this->getChildren = $ref->newInstance($child, $this->flags);
			}
			catch(Exception $e)
			{
				if (!$this->flags & self::CATCH_GET_CHILD)
				{
					throw $e;
				}
				$this->hasChildren = false;
				$this->getChildren = NULL;
			}
		} else
		{
			$this->getChildren = NULL;
		}
		parent::next();
	}
	
	private $ref;

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