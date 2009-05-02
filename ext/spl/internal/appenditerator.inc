<?php

/** @file appenditerator.inc
 * @ingroup SPL
 * @brief class AppendIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   Iterator that iterates over several iterators one after the other
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1
 */
class AppendIterator implements OuterIterator
{
	/** @internal array of inner iterators */
	private $iterators;

	/** Construct an empty AppendIterator
	 */
	function __construct()
	{
		$this->iterators = new ArrayIterator();
	}

	/** Append an Iterator
	 * @param $it Iterator to append
	 *
	 * If the current state is invalid but the appended iterator is valid
	 * the the AppendIterator itself becomes valid. However there will be no
	 * call to $it->rewind(). Also if the current state is invalid the inner
	 * ArrayIterator will be rewound und forwarded to the appended element.
	 */	
	function append(Iterator $it)
	{
		$this->iterators->append($it);
	}

	/** @return the current inner Iterator
	 */
	function getInnerIterator()
	{
		return $this->iterators->current();
	}

	/** Rewind to the first element of the first inner Iterator.
	 * @return void
	 */
	function rewind()
	{
		$this->iterators->rewind();
		if ($this->iterators->valid())
		{
			$this->getInnerIterator()->rewind();
		}
	}

	/** @return whether the current element is valid
	  */
	function valid()
	{
		return $this->iterators->valid() && $this->getInnerIterator()->valid();
	}

	/** @return the current value if it is valid or \c NULL
	 */
	function current()
	{
		/* Using $this->valid() would be exactly the same; it would omit
		 * the access to a non valid element in the inner iterator. Since
		 * the user didn't respect the valid() return value false this
		 * must be intended hence we go on. */
		return $this->iterators->valid() ? $this->getInnerIterator()->current() : NULL;
	}

	/** @return the current key if it is valid or \c NULL
	 */
	function key()
	{
		return $this->iterators->valid() ? $this->getInnerIterator()->key() : NULL;
	}

	/** Move to the next element. If this means to another Iterator that 
	 * rewind that Iterator.
	 * @return void
	 */
	function next()
	{
		if (!$this->iterators->valid())
		{
			return; /* done all */
		}
		$this->getInnerIterator()->next();
		if ($this->getInnerIterator()->valid())
		{
			return; /* found valid element in current inner iterator */
		}
		$this->iterators->next();
		while ($this->iterators->valid())
		{
			$this->getInnerIterator()->rewind();
			if ($this->getInnerIterator()->valid())
			{
				return; /* found element as first elemet in another iterator */
			}
			$this->iterators->next();
		}
	}

	/** Aggregates the inner iterator
	 */	
	function __call($func, $params)
	{
		return call_user_func_array(array($this->getInnerIterator(), $func), $params);
	}
}

?>