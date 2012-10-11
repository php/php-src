<?php

/** @file emptyiterator.inc
 * @ingroup SPL
 * @brief class EmptyIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   An empty Iterator
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1
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