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
	const BYPASS_CURRENT = 0x00000004;
	const BYPASS_KEY     = 0x00000008;

	private $rit_flags;

	/**
	 * @param it         iterator to use as inner iterator
	 * @param rit_flags  flags passed to RecursiveIteratoIterator (parent)
	 * @param cit_flags  flags passed to RecursiveCachingIterator (for hasNext)
	 * @param mode       mode  passed to RecursiveIteratoIterator (parent)
	 */
	function __construct(RecursiveIterator $it, $rit_flags = self::BYPASS_KEY, $cit_flags = CachingIterator::CATCH_GET_CHILD, $mode = self::SELF_FIRST)
	{
		parent::__construct(new RecursiveCachingIterator($it, $cit_flags), $mode, $rit_flags);
		$this->rit_flags = $rit_flags;
	}

	/**  Prefix strings used in getPrefix()
	 *
	 * 0: prefix used to start elements
	 * 1: prefix used if $level < depth and hasNext($level) == true
	 * 2: prefix used if $level < depth and hasNext($level) == false
	 * 3: prefix used if $level == depth and hasNext($level) == true
	 * 4: prefix used if $level == depth and hasNext($level) == false
	 * 5: prefix used right in front of the current element
	 */
	public $prefix = array(0=>'', 1=>'| ', 2=>'  ', 3=>'|-', 4=>'\-', 5=>'');

	/** @return string to place in front of current element
	 */
	function getPrefix()
	{
		$tree = '';
		for ($level = 0; $level < $this->getDepth(); $level++)
		{
			$tree .= $this->getSubIterator($level)->hasNext() ? $this->prefix[1] : $this->prefix[2];
		}
		$tree .= $this->getSubIterator($level)->hasNext() ? $this->prefix[3] : $this->prefix[4];

		return $this->prefix[0] . $tree . $this->prefix[5];
	}

	/** @return string presentation build for current element
	 */
	function getEntry()
	{
		return @(string)parent::current();
	}

	/** @return string to place after the current element
	 */
	function getPostfix()
	{
		return '';
	}

	/** @return the current element prefixed and postfixed
	 */
	function current()
	{
		if ($this->rit_flags & self::BYPASS_CURRENT)
		{
			return parent::current();
		}
		else
		{
			return $this->getPrefix() . $this->getEntry() .  $this->getPostfix();
		}
	}

	/** @return the current key prefixed and postfixed
	 */
	function key()
	{
		if ($this->rit_flags & self::BYPASS_KEY)
		{
			return parent::key();
		}
		else
		{
			return $this->getPrefix() . parent::key() .  $this->getPostfix();
		}
	}

	/** Aggregates the inner iterator
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->getSubIterator(), $func), $params);
	}
}

?>