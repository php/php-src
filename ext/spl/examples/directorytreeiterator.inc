<?php

/** @file directorytreeiterator.inc
 * @ingroup Examples
 * @brief class DirectoryTreeIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   DirectoryIterator to generate ASCII graphic directory trees
 * @author  Marcus Boerger
 * @version 1.0
 */
class DirectoryTreeIterator extends RecursiveIteratorIterator
{
	/** Construct from a path.
	 * @param $path directory to iterate
	 */
	function __construct($path)
	{
		parent::__construct(new CachingRecursiveIterator(new RecursiveDirectoryIterator($path), CIT_CALL_TOSTRING|CIT_CATCH_GET_CHILD), 1);
	}

	/** @return the current element prefixed with ASCII graphics
	 */	
	function current()
	{
		$tree = '';
		for ($l=0; $l < $this->getDepth(); $l++) {
			$tree .= $this->getSubIterator($l)->hasNext() ? '| ' : '  ';
		}
		return $tree . ($this->getSubIterator($l)->hasNext() ? '|-' : '\-') 
		       . $this->getSubIterator($l)->__toString();
	}

	/** Aggregates the inner iterator
	 */	
	function __call($func, $params)
	{
		return call_user_func_array(array($this->getSubIterator(), $func), $params);
	}
}

?>