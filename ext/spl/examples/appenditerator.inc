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
	}

	function valid()
	{
		return $this->iterators->valid() && $this->getInnerIterator()->valid();
	}

	function current()
	{
		return $this->getInnerIterator()->current();
	}

	function key()
	{
		return $this->getInnerIterator()->key();
	}
	
	function next()
	{
		while($this->iterators->valid()) {
			$this->getInnerIterator()->next();
			if ($this->valid()) {
				return;
			}
			$this->iterators->next();
		}
	}
}

?>