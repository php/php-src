<?php

abstract class SearchIterator extends FilterIterator
{
	private $done = false;

	function rewind() {
		parent::rewind();
		$this->done = false;
	}

	function valid() {
		return !$this->done && parent::valid();
	}
	
	function next() {
		$this->done = true;
	}
}

?>