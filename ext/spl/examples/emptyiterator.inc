<?php

/** \ingroup Examples
 * @brief   An empty Iterator
 * @author  Marcus Boerger
 * @version 1.0
 *
 */
class EmptyIterator implements Iterator
{
	/** No operation.
	 * @return void
	 */
	function rewind()
	{
		// nothing to do
	}

	/** @return \c false
	 */
	function valid()
	{
		return false;
	}

	/** This function must not be called. It throws an exception upon access.
	 * @throw Exception
	 * @return void
	 */
	function current()
	{
		throw new Exception('Accessing the value of an EmptyIterator');
	}

	/** This function must not be called. It throws an exception upon access.
	 * @throw Exception
	 * @return void
	 */
	function key()
	{
		throw new Exception('Accessing the key of an EmptyIterator');
	}

	/** No operation.
	 * @return void
	 */
	function next()
	{
		// nothing to do
	}
}

?>