<?php

/** @file filteriterator.inc
 * @ingroup SPL
 * @brief class FilterIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Abstract filter for iterators
 * @author  Marcus Boerger
 * @version 1.1
 * @since PHP 5.0
 *
 * Instances of this class act as a filter around iterators. In other words
 * you can put an iterator into the constructor and the instance will only
 * return selected (accepted) elements.
 *
 * The only thing that needs to be done to make this work is implementing
 * method accept(). Typically this invloves reading the current element or
 * key of the inner Iterator and checking whether it is acceptable.
 */
abstract class FilterIterator implements OuterIterator
{
	private $it;

	/**
	 * Constructs a filter around another iterator.
	 *
	 * @param it     Iterator to filter
	 */
	function __construct(Iterator $it) {
		$this->it = $it;
	}

	/**
	 * Rewind the inner iterator.
	 */
	function rewind() {
		$this->it->rewind();
		$this->fetch();
	}

	/**
	 * Accept function to decide whether an element of the inner iterator
	 * should be accessible through the Filteriterator.
	 *
	 * @return whether or not to expose the current element of the inner
	 *         iterator.
	 */
	abstract function accept();

	/**
	 * Fetch next element and store it.
	 *
	 * @return void
	 */
	protected function fetch() {
		while ($this->it->valid()) {
			if ($this->accept()) {
				return;
			}
			$this->it->next();
		};
	}

	/**
	 * Move to next element
	 *
	 * @return void
	 */
	function next() {
		$this->it->next();
		$this->fetch();
	}

	/**
	 * @return Whether more elements are available
	 */
	function valid() {
		return $this->it->valid();
	}

	/**
	 * @return The current key
	 */
	function key() {
		return $this->it->key();
	}

	/**
	 * @return The current value
	 */
	function current() {
		return $this->it->current();
	}

	/**
	 * hidden __clone
	 */
	protected function __clone() {
		// disallow clone
	}

	/**
	 * @return The inner iterator
	 */
	function getInnerIterator()
	{
		return $this->it;
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
}

?>
