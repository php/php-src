<?php

/**
 * @brief   Regular expression filter for string iterators
 * @author  Marcus Boerger
 * @version 1.0
 *
 * Instances of this class act as a filter around iterators whose elements
 * are strings. In other words you can put an iterator into the constructor
 * and the instance will only return elements which match the given regular 
 * expression.
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
	 * @patam regex  Regular expression used as a filter.
	 */
	function __construct(Iterator $it) {
		$this->it = $it;
	}

	function rewind() {	
		$this->it->rewind();
		$this->fetch();
	}

	abstract function accept();

	/**
	 * Fetch next element and store it.
	 *
	 * @return void
	 */
	protected function fetch() {
		while ($this->it->hasMore()) {
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
	function hasMore() {
		return $this->it->hasMore();
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