<?php

/**
 * @brief   Regular expression filter for string iterators
 * @author  Marcus Boerger
 * @version 1.0
 *
 * Instances of this class act as a filter around iterators. In other words 
 * you can put an iterator into the constructor and the instance will only 
 * return selected (accepted) elements.
 */
abstract class FilterIterator implements Iterator
{
	protected $it;

	/**
	 * Constructs a filter around an iterator whose elemnts are strings.
	 * If the given iterator is of type spl_sequence then its rewind()
	 * method is called.
	 *
	 * @param it     Object that implements at least spl_forward
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
}

?>