<?php

/** @file directoryfilterdots.inc
 * @ingroup Examples
 * @brief class DirectoryFilterDots
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   A filtered DirectoryIterator
 * @author  Marcus Boerger
 * @version 1.0
 *
 * This Iteraotr takes a pathname from which it creates a DirectoryIterator
 * and makes it recursive. Further more it filters the entries '.' and '..'.
 */
class DirectoryFilterDots extends FilterIterator implements RecursiveIterator
{
	/** Construct from a path.
	 * @param $path directory to iterate
	 */
	function __construct($path)
	{
		parent::__construct(new DirectoryIterator($path));
	}

	/** @return whether the current entry is neither '.' nor '..'
	 */	
	function accept()
	{
		return !$this->it->isDot();
	}
	
	/** @return whether the current entry is a directory
	 */
	function hasChildren()
	{
		return $this->it->hasChildren();
	}

	/** @return the current subdirectory as a new DirectoryFilterDots instance.
	 */
	function getChildren()
	{
		return new DirectoryFilterDots($this->it->getPathname());
	}
	
	/** @return the current entries path name
	 */
	function key()
	{
		return $this->it->getPathname();
	}
}

?>