<?php

/** @file directorytree.inc
 * @ingroup Examples
 * @brief class DirectoryTree
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   A directory iterator that does not show '.' and '..'.
 * @author  Marcus Boerger
 * @version 1.0
 */
class DirectoryTree extends RecursiveIteratorIterator
{
	/** Construct from a path.
	 * @param $path directory to iterate
	 */
	function __construct($path) {
		parent::__construct(new DirectoryFilterDots($path));
	}
}

?>