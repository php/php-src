<?php

/**
 * @brief   Iterator that iterates over several iterators one after the other
 * @author  Marcus Boerger
 * @version 1.0
 *
 */
class AppendIterator implements Iterator
{
	protected $iterators;

	function __construct()
	{
		$this->iterators = new ArrayIterator();
	}
	
	function append(Iterator $it)
	{
		$this->iterators->append($it);
	}

	function getInnerIterator()
	{
		return $this->iterators->current();
	}

	function rewind()
	{
		$this->iterators->rewind();
		if ($this->iterators->valid())
		{
			$this->iterators->rewind();
		}
	}

	function valid()
	{
		return $this->iterators->valid() && $this->getInnerIterator()->valid();
	}

	function current()
	{
		/* Using $this->valid() would be exactly the same; it would omit
		 * the access to a non valid element in the inner iterator. Since
		 * the user didn't respect the valid() return value false this
		 * must be intended hence we go on. */
		return $this->iterators->valid() ? $this->getInnerIterator()->current() : NULL;
	}

	function key()
	{
		return $this->iterators->valid() ? $this->getInnerIterator()->key() : NULL;
	}
	
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
}

?>