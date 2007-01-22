<?php

/** @file directoryfilterdots.inc
 * @ingroup Examples
 * @brief class DirectoryFilterDots
 * @author  Marcus Boerger
 * @date    2003 - 2006
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   A filtered DirectoryIterator
 * @author  Marcus Boerger
 * @version 1.2
 *
 * This Iterator takes a pathname from which it creates a RecursiveDirectoryIterator
 * and makes it recursive. Further more it filters the entries '.' and '..'.
 */
class DirectoryFilterDots extends RecursiveFilterIterator
{
	/** Construct from a path.
	 * @param $path directory to iterate
	 */
	function __construct($path)
	{
		parent::__construct(new RecursiveDirectoryIterator($path));
	}

	/** @return whether the current entry is neither '.' nor '..'
	 */	
	function accept()
	{
		return !$this->getInnerIterator()->isDot();
	}

	/** @return the current entries path name
	 */
	function key()
	{
		return $this->getInnerIterator()->getPathname();
	}
}

?>
