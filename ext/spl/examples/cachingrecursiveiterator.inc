<?php

class CachingRecursiveIterator extends CachingIterator implements RecursiveIterator
{
	protected $hasChildren;
	protected $getChildren;
	protected $catch_get_child_exceptions;

	function __construct(RecursiveIterator $it, $catch_get_child_exceptions = false) {
		$this->catch_get_child_exceptions = $catch_get_child_exceptions;
		parent::__construct($it);
	}
	
	function next() {
		if ($this->hasChildren = $this->it->hasChildren()) {
			try {
				//$this->getChildren = new CachingRecursiveIterator($this->it->getChildren(), $this->catch_get_child_exceptions);
				// workaround memleaks...
				$child = $this->it->getChildren();
				$this->getChildren = new CachingRecursiveIterator($child, $this->catch_get_child_exceptions);
			}
			catch(Exception $e) {
				if (!$this->catch_get_child_exceptions) {
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

	function hasChildren() {
		return $this->hasChildren;
	}

	function getChildren() {
		return $this->getChildren;
	}
}

?>