<?php

class SeekableIterator implements Iterator
{
	function seek($index) {
		$this->rewind();
		$position = 0;
		while($position < $index && $this->hasMore()) {
			$this->next();
			$position++;
		}
		return $position;
	}
}

?>