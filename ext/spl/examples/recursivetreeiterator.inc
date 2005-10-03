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

	/** @return prefix used if $level < depth and hasNext($level) == true
	 */
	function getPrefix1()
	{
		return '| ';
	}

	/** @return prefix used if $level < depth and hasNext($level) == false
	 */
	function getPrefix2()
	{
		return '  ';
	}

	/** @return prefix used if $level == depth and hasNext($level) == true
	 */
	function getPrefix3()
	{
		return '|-';
	}

	/** @return prefix used if $level == depth and hasNext($level) == false
	 */
	function getPrefix4()
	{
		return '\-';
	}

	function getPrefix($level)
	{
		if ($level < 0 || $level > $this->getDepth())
		{
			throw new OutOfBoundsException('Parameter $level must be >= 0 and <= depth');
		}
		if ($level < $this->getDepth())
		{
			return $this->getSubIterator($level)->hasNext() ? $this->getPrefix1() : $this->getPrefix2();
		}
		else
		{
			return $this->getSubIterator($level)->hasNext() ? $this->getPrefix3() : $this->getPrefix4();
		}
	}

	/** @return the current element prefixed with ASCII graphics
	 */
	function current()
	{
		$tree = '';
		for ($l=0; $l <= $this->getDepth(); $l++)
		{
			$tree .= $this->getPrefix($l);
		}
		
		return $tree . ($this->callToString ? $this->__toString() : parent::current());
	}

	/** Aggregates the inner iterator
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->getSubIterator(), $func), $params);
	}
}
