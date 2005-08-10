<?php

/** @file recursivefilteriterator.inc
 * @ingroup SPL
 * @brief class RecursiveFilterIterator
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Iterator to filter recursive iterators
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 6.0
 *
 * This extended FilterIterator allows a recursive iteration using 
 * RecursiveIteratorIterator that only shows those elements which
 * are accepted. It is of course better to filter before doing the 
 * recursion since it allows to prevent recursion into children that 
 * get declined later anyway.
 */
class RecursiveFilterIterator extends FilterIterator implements RecursiveIterator
{
	/** @param $it the RecursiveIterator to filter
	 */
	function __construct(RecursiveIterator $it)
	{
		parent::__construct($it);
	}
	
	/** @return whether the current element has children
	 */
	function hasChildren()
	{
		return $this->it->hasChildren();
	}

	/** @return the ParentIterator for the current elements children
	 */
	function getChildren()
	{
		return new RecursiveFilterIterator($this->it->getChildren());
	}
}

?>