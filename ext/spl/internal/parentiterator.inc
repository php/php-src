<?php

/** @file parentiterator.inc
 * @ingroup SPL
 * @brief class FilterIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
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
	/** @return whetehr the current element has children
	 */
	function accept()
	{
		return $this->it->hasChildren();
	}
}

?>