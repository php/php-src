<?php

/** @file norewinditerator.inc
 * @ingroup Examples
 * @brief class NoRewindIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   A recursive Filter
 * @author  Marcus Boerger
 * @version 1.0
 *
 */
abstract class RecursiveFilterIterator extends FilterIterator implements RecursiveIterator
{
	function __construct(RecursiveIterator $it)
	{
		parent::__construct($it);
	}
	
	function hasChildren()
	{
		return $this->getInnerIterator()->hasChildren();
	}

	function getChildren()
	{
		return $this->getInnerIterator()->getChildren();
	}
}

?>