<?php

class CachingRecursiveIterator extends CachingIterator implements RecursiveIterator
{
	protected $hasChildren;
	protected $getChildren;

	function __construct(RecursiveIterator $it, $flags = CIT_CALL_TOSTRING)
	{
		parent::__construct($it, $flags);
	}
	
	function rewind();
	{
	   $this->hasChildren = false;
	   $this->getChildren = NULL;
	   parent::rewind();
	}

	function next()
	{
		if ($this->hasChildren = $this->it->hasChildren()) {
			try {
				//$this->getChildren = new CachingRecursiveIterator($this->it->getChildren(), $this->flags);
				// workaround memleaks...
				$child = $this->it->getChildren();
				$this->getChildren = new CachingRecursiveIterator($child, $this->flags);
			}
			catch(Exception $e) {
				if (!$this->flags & CIT_CATCH_GET_CHILD) {
					throw $e;
				}
				$this->hasChildren = false;
				$this->getChildren = NULL;
			}
		} else {
			$this->getChildren = NULL;
		}
		parent::next();
	}

	function hasChildren()
	{
		return $this->hasChildren;
	}

	function getChildren()
	{
		return $this->getChildren;
	}
}

?>