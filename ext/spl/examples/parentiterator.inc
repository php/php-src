<?php

class ParentIterator extends FilterIterator implements RecursiveIterator
{
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