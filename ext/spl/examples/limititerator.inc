<?php

class LimitIterator implements Iterator
{
	protected $it;
	protected $offset;
	protected $count;
	private $pos;

	// count === NULL means all
	function __construct(Iterator $it, $offset = 0, $count = -1)
	{
		if ($offset < 0) {
			throw new exception('Parameter offset must be > 0');
		}
		if ($count < 0 && $count != -1) {
			throw new exception('Parameter count must either be -1 or a value greater than or equal to 0');
		}
		$this->it     = $it;
		$this->offset = $offset;
		$this->count  = $count;
		$this->pos    = 0;
	}
	
	function seek($position) {
		if ($position < $this->offset) {
			throw new exception('Cannot seek to '.$position.' which is below offset '.$this->offset);
		}
		if ($position > $this->offset + $this->count && $this->count != -1) {
			throw new exception('Cannot seek to '.$position.' which is behind offset '.$this->offset.' plus count '.$this->count);
		}
		if ($this->it instanceof SeekableIterator) {
			$this->it->seek($position);
			$this->pos = $position;
		} else {
			while($this->pos < $position && $this->it->valid()) {
				$this->next();
			}
		}
	}

	function rewind()
	{
		$this->it->rewind();
		$this->pos = 0;
		$this->seek($this->offset);
	}
	
	function valid() {
		return ($this->count == -1 || $this->pos < $this->offset + $this->count)
			 && $this->it->valid();
	}
	
	function key() {
		return $this->it->key();
	}

	function current() {
		return $this->it->current();
	}

	function next() {
		$this->it->next();
		$this->pos++;
	}

	function getPosition() {
		return $this->pos;
	}
}

?>