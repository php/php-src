<?php

/** @file infiniteiterator.inc
 * @ingroup Examples
 * @brief class InfiniteIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   An infinite Iterator
 * @author  Marcus Boerger
 * @version 1.0
 *
 * This Iterator takes another Iterator and infinitvely iterates it by
 * rewinding it when its end is reached.
 *
 * \note Even an InfiniteIterator stops if its inner Iterator is empty.
 *
 \verbatim
 $it       = new ArrayIterator(array(1,2,3));
 $infinite = new InfiniteIterator($it);
 $limit    = new LimitIterator($infinite, 0, 5);
 foreach($limit as $val=>$key)
 {
 	echo "$val=>$key\n";
 }
 \endverbatim
 */
class InfiniteIterator implements Iterator
{
	/** @internal 
	 * The inner Iterator. */
	private $it;

	/** Construct from another Iterator.
	 * @param $it the inner Iterator.
	 */
	function __construct(Iterator $it)
	{
		$this->it = $it;
	}

	/** @return the inner iterator
	 */
	function getInnerIterator()
	{
		return $this->it;
	}

	/** Rewind the inner iterator.
	 * @return void
	 */
	function rewind()
	{
		$this->it->rewind();
	}

	/** @return whether the current element is valid
	 */
	function valid()
	{
		return $this->it->valid();
	}

	/** @return the current value
	 */
	function current()
	{
		return $this->it->current();
	}

	/** @return the current key
	 */
	function key()
	{
		return $this->it->key();
	}

	/** Move the inner Iterator forward to its next element or rewind it.
	 * @return void
	 */
	function next()
	{
		$this->it->next();
		if (!$this->it->valid())
		{
			$this->it->rewind();
		}
	}

	/** Aggregates the inner iterator
	 */	
	function __call($func, $params)
	{
		return call_user_func_array(array($this->it, $func), $params);
	}
}

?>