<?php

abstract class SearchIterator extends FilterIterator
{
	private $done = false;

	function rewind() {
		parent::rewind();
		$this->done = false;
	}

	function hasMore() {
		return !$this->done && parent::hasMore();
	}
	
	function next() {
		$this->done = true;
	}
}

?>