<?php

/** @file iteratoriterator.inc
 * @ingroup SPL
 * @brief class IteratorIterator
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief Basic Iterator wrapper
 * @since PHP 5.1
 */
class IteratorIterator implements OuterIterator
{
	/** Construct an IteratorIterator from an Iterator or an IteratorAggregate.
	 *
	 * Classes that only implement Traversable can be wrapped only after 
	 * converting class IteratorIterator into c code.
	 */
	function __construct(Traversable $iterator)
	{
		if ($iterator instanceof IteratorAggregate)
		{
		    $iterator = $iterator->getIterator();
		}
		if ($iterator instanceof Iterator)
		{
			$this->iterator = $iterator;
		}
		else
		{
			throw new Exception("Classes that only implement Traversable can be wrapped only after converting class IteratorItaerator into c code");
		}
	}

	/** \return the inner iterator as passed to the constructor
	 */
	function getInnerIterator()
	{
		return $this->iterator;
	}

	/** \return whether the iterator is valid
	 */
	function valid()
	{
		return $this->iterator->valid();
	}

	/** \return current key
	 */
	function key()
	{
		return $this->iterator->key();
	}

	/** \return current value
	 */
	function current()
	{
		return $this->iterator->current();
	}

	/** forward to next element
	 */
	function next()
	{
		return $this->iterator->next();
	}

	/** rewind to the first element
	 */
	function rewind()
	{
		return $this->iterator->rewind();
	}

	/** Aggregate the inner iterator
	 *
	 * @param func    Name of method to invoke
	 * @param params  Array of parameters to pass to method
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->it, $func), $params);
	}

	/** The inner iterator must be private because when this class will be
	 * converted to c code it won't no longer be available.
	 */
	private $iterator;
}

?>