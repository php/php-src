<?php

class DirectoryFilterDots extends FilterIterator implements RecursiveIterator
{
	function __construct($path) {
		parent::__construct(new DirectoryIterator($path));
	}
	
	function accept() {
		return !$this->it->isDot();
	}
	
	function hasChildren() {
		return $this->it->hasChildren();
	}

	function getChildren() {
		return new DirectoryFilterDots($this->it->getPathname());
	}
	
	function key() {
		return $this->it->getPathname();
	}
}

?>