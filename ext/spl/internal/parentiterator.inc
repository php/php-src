<?php

class ParentIterator extends FilterIterator implements RecursiveIterator
{
	function __construct(RecursiveIterator $it)
	{
		parent::__construct($it);
	}
	function accept()
	{
		return $this->it->hasChildren();
	}

	function hasChildren()
	{
		return $this->it->hasChildren();
	}

	function getChildren()
	{
		return new ParentIterator($this->it->getChildren());
	}
}

?>