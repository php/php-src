<?php

/**
 * @brief   An Iterator that doesn't call rewind
 * @author  Marcus Boerger
 * @version 1.0
 *
 */
class NoRewindIterator implements Iterator
{
	protected $it;
	
	function __construct(Iterator $it)
	{
		$this->it = $it;
	}

	function rewind()
	{
		// nothing to do
	}

	function valid()
	{
		return $this->it->valid();
	}

	function current()
	{
		return $this->it->current();
	}

	function key()
	{
		return $this->it->key();
	}

	function next()
	{
		$this->it->next();
	}
}

?>