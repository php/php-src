<?php

/** @file parentiterator.inc
 * @ingroup SPL
 * @brief class FilterIterator
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Iterator to filter parents
 * @author  Marcus Boerger
 * @version 1.1
 * @since PHP 5.0
 *
 * This extended FilterIterator allows a recursive iteration using 
 * RecursiveIteratorIterator that only shows those elements which have 
 * children.
 */
class ParentIterator extends FilterIterator implements RecursiveIterator
{
	/** @param $it the RecursiveIterator to filter
	 */
	function __construct(RecursiveIterator $it)
	{
		parent::__construct($it);
	}
	
	/** @return whetehr the current element has children
	 */
	function accept()
	{
		return $this->it->hasChildren();
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
		return new ParentIterator($this->it->getChildren());
	}
}

?>