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
class KeyFilter implements Iterator
{
	protected $it;
	protected $regex;
	protected $key;
	protected $curr;

	/**
	 * Constructs a filter around an iterator whose elemnts are strings.
	 * If the given iterator is of type spl_sequence then its rewind()
	 * method is called.
	 *
	 * @param it     Object that implements at least spl_forward
	 * @patam regex  Regular expression used as a filter.
	 */
	function __construct(Iterator $it, $regex) {
		$this->it = $it;
		$this->regex = $regex;
		$this->fetch();
	}

	/**
	 * Rewind input iterator
	 */
	function rewind() {
		$this->it->rewind();
	}
	
	/**
	 * Destruct the iterator.
	 */
	function __destruct() {
		unset($this->it);
	}

	/**
	 * Fetch next element and store it.
	 *
	 * @return void
	 */
	protected function fetch() {
		$this->key = false;
		$this->curr = false;
		while ($this->it->hasMore()) {
			$key = $this->it->key();
			if (ereg($this->regex, $key)) {
				$this->key = $key;
				$this->curr = $this->it->current();
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
		return $this->key !== false;
	}
	
	/**
	 * @return The current key
	 */
	function key() {
		return $this->key;
	}
	
	/**
	 * @return The current value
	 */
	function current() {
		return $this->curr;
	}
	
	/**
	 * hidden __clone
	 */
	protected function __clone() {
		// disallow clone 
	}
}

?>