<?php

/** @file recursiveiterator.inc
 * @ingroup SPL
 * @brief class RecursiveIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Interface for recursive iteration with RecursiveIteratorIterator
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.0
 */
interface RecursiveIterator extends Iterator
{
	/** @return whether the current element has children
	 */
	function hasChildren();

	/** @return the sub iterator for the current element
	 * @note The returned object must implement RecursiveIterator.
	 */
	function getChildren();
}

?>
