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
 * @version 1.2
 * @since PHP 5.1
 *
 * This extended FilterIterator allows a recursive iteration using 
 * RecursiveIteratorIterator that only shows those elements which have 
 * children.
 */
class ParentIterator extends RecursiveFilterIterator
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

	/** @return the ParentIterator for the current elements children
	 */
	function getChildren()
	{
		return new ParentIterator($this->it->getChildren());
	}
}

?>