<?php

/** @file directorytreeiterator.inc
 * @ingroup Examples
 * @brief class DirectoryTreeIterator
 * @author  Marcus Boerger
 * @date    2003 - 2008
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   DirectoryIterator to generate ASCII graphic directory trees
 * @author  Marcus Boerger
 * @version 1.1
 */
class DirectoryTreeIterator extends RecursiveIteratorIterator
{
	/** Construct from a path.
	 * @param $path directory to iterate
	 */
	function __construct($path)
	{
		parent::__construct(
			new RecursiveCachingIterator(
				new RecursiveDirectoryIterator($path, RecursiveDirectoryIterator::KEY_AS_FILENAME
				),
				CachingIterator::CALL_TOSTRING|CachingIterator::CATCH_GET_CHILD
			),
			parent::SELF_FIRST
		);
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
