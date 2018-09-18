<?php

/** @file searchiterator.inc
 * @ingroup Examples
 * @brief abstract class SearchIterator
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief Iterator to search for a specific element
 * @author  Marcus Boerger
 * @version 1.0
 *
 * This extended FilterIterator stops after finding the first acceptable
 * value.
 */
abstract class SearchIterator extends FilterIterator
{
	/** @internal whether an entry was found already */
	private $done = false;

	/** Rewind and reset so that it once again searches.
	 * @return void
	 */
	function rewind()
	{
		parent::rewind();
		$this->done = false;
	}

	/** @return whether the current element is valid
	 * which can only happen once per iteration.
	 */
	function valid()
	{
		return !$this->done && parent::valid();
	}

	/** Do not move forward but instead mark as finished.
	 * @return void
	 */
	function next()
	{
		$this->done = true;
	}

	/** Aggregates the inner iterator
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->getInnerIterator(), $func), $params);
	}
}

?>
