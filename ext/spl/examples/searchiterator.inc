<?php

abstract class SearchIterator extends FilterIterator
{
	private $done = false;

	function hasMore() {
		return !$this->done && parent::hasMore();
	}
	
	function next() {
		$this->done = true;
	}
}

?>