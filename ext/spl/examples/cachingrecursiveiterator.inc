<?php

class CachingRecursiveIterator extends CachingIterator implements RecursiveIterator
{
	protected $hasChildren;
	protected $getChildren;
	protected $catch_get_child;

	function __construct(RecursiveIterator $it, $catch_get_child = false)
	{
		$this->catch_get_child = $catch_get_child;
		parent::__construct($it);
	}
	
	function next()
	{
		if ($this->hasChildren = $this->it->hasChildren()) {
			try {
				//$this->getChildren = new CachingRecursiveIterator($this->it->getChildren(), $this->catch_get_child);
				// workaround memleaks...
				$child = $this->it->getChildren();
				$this->getChildren = new CachingRecursiveIterator($child, $this->catch_get_child);
			}
			catch(Exception $e) {
				if (!$this->catch_get_child) {
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