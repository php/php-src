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
class filter implements spl_forward
{
	protected $it;
	protected $regex;
	protected $curr;

	/**
	 * Constructs a filter around an iterator whose elemnts are strings.
	 * If the given iterator is of type spl_sequence then its rewind()
	 * method is called.
	 *
	 * @param it     Object that implements at least spl_forward
	 * @patam regex  Regular expression used as a filter.
	 */
	function __construct(spl_forward $it, $regex) {
		if ($it instanceof spl_sequence) {
			$it->rewind();
		}
		$this->it = $it;
		$this->regex = $regex;
		$this->fetch();
	}
	
	/**
	 * Destruct the iterator.
	 */
	function __destruct() {
		unset($this->it);
	}

	protected function accept($curr) {
		return ereg($this->regex, $curr);
	}

	/**
	 * Fetch next element and store it.
	 *
	 * @return void
	 */
	protected function fetch() {
		$this->curr = false;
		while ($this->it->hasMore()) {
			$curr = $this->it->current();
			if ($this->accept($curr)) {
				$this->curr = $curr;
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
		return $this->curr !== false;
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