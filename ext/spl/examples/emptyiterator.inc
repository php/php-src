<?php

/**
 * @brief   An empty Iterator
 * @author  Marcus Boerger
 * @version 1.0
 *
 */
class EmptyIterator implements Iterator
{
	function rewind()
	{
		// nothing to do
	}

	function valid()
	{
		return false;
	}

	function current()
	{
		throw new Exception('Accessing the value of an EmptyIterator');
	}

	function key()
	{
		throw new Exception('Accessing the key of an EmptyIterator');
	}

	function next()
	{
		// nothing to do
	}
}

?>