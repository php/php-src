<?php

class LimitIterator implements Iterator
{
	protected $it;
	protected $offset;
	protected $count;
	protected $index;

	// negative offset is  respected
	// count === NULL means all
	function __construct(Iterator $it, $offset = 0, $count = NULL)
	{
		$this->it     = $it;
		$this->offset = $offset;
		$this->count  = $count;
		$this->index  = 0;
	}
	
	function rewind()
	{
		$this->it->rewind();
		$this->index = 0;
		if (is_a($this->it, 'SeekableIterator')) {
			$this->index = $this->it->seek($this->offset);
		} else {
			while($this->index < $this->offset && $this->it->hasMore()) {
				$this->next();
			}
		}
	}
	
	function hasMore() {
		return (is_null($this->count) || $this->index < $this->offset + $this->count)
			 && $this->it->hasMore();
	}
	
	function key() {
		return $this->it->key();
	}

	function current() {
		return $this->it->current();
	}

	function next() {
		$this->it->next();
		$this->index++;
	}
}

?>