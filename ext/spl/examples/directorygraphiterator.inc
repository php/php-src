<?php

/** @file directorygraphiterator.inc
 * @ingroup Examples
 * @brief class DirectoryGraphIterator
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   A tree iterator that only shows directories.
 * @author  Marcus Boerger
 * @version 1.0
 */
class DirectoryGraphIterator extends DirectoryTreeIterator
{
	function __construct($path)
	{
		RecursiveIteratorIterator::__construct(new RecursiveCachingIterator(new ParentIterator(new RecursiveDirectoryIterator($path)), CachingIterator::CALL_TOSTRING|CachingIterator::CATCH_GET_CHILD), 1);
	}
}

?>