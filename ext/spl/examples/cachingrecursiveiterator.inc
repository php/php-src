<?php

class CachingRecursiveIterator extends CachingIterator implements RecursiveIterator
{
	protected $hasChildren;
	protected $getChildren;

	function __construct(RecursiveIterator $it) {
		parent::__construct($it);
	}
	
	function next() {
		if ($this->hasChildren = $this->it->hasChildren()) {
			$this->getChildren = new CachingRecursiveIterator($this->it->getChildren());
		} else {
			$this->getChildren = NULL;
		}
		parent::next();
	}

	function hasChildren() {
		return $this->hasChildren;
	}

	function getChildren() {
		return $this->getChildren;
	}
}

?>