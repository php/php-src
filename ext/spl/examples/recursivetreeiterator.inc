<?php

/** @file recursivetreeiterator.inc
 * @ingroup Examples
 * @brief   class RecursiveTreeIterator
 * @author  Marcus Boerger, Johannes Schlueter
 * @date    2005
 *
 * SPL - Standard PHP Library
 */


/** @ingroup Examples
 * @brief   RecursiveIteratorIterator to generate ASCII graphic trees for the
 *          entries in a RecursiveIterator
 * @author  Marcus Boerger, Johannes Schlueter
 * @version 1.0
 */
class RecursiveTreeIterator extends RecursiveIteratorIterator
{
	private $callToString;
	
	function __construct(RecursiveIterator $it, $cit_flags = CachingIterator::CATCH_GET_CHILD)
	{
		parent::__construct(new RecursiveCachingIterator($it, $cit_flags), 1);
		$this->callToString = (bool)($cit_flags & CachingIterator::CALL_TOSTRING);
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
		       . ($this->callToString ? $this->getSubIterator($l)->__toString() : $this->getSubIterator($l)->current());
	}

	/** Aggregates the inner iterator
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->getSubIterator(), $func), $params);
	}
}
