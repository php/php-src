<?php

/**
 * @brief   Iterates through recursive iterators
 * @author  Marcus Boerger
 * @version 1.0
 *
 */
class RecursiveIteratorIterator implements Iterator
{
	protected $ait = array();
	protected $count = 0;

	function __construct(RecursiveIterator $it) {
		$this->count = 1;
		$this->ait[0] = $it;
	}


	function rewind() {
		while ($this->count > 1) {
			unset($this->ait[--$this->count]);
		}
		$this->ait[0]->rewind();
		$this->ait[0]->recursed = false;
	}
	
	function hasMore() {
		$count = $this->count;
		while ($count--) {
			$it = $this->ait[$count];
			if ($it->hasMore()) {// || (!$it->recursed && $it->isRecursive())) {
				return true;
			}
		}
		return false;
	}
	
	function key() {
		$it = $this->ait[$this->count-1];
		return $it->key();
	}
	
	function current() {
		$it = $this->ait[$this->count-1];
		return $it->current();
	}
	
	function next() {
		while ($this->count) {
			$it = $this->ait[$this->count-1];
			if ($it->hasMore()) {
				if (!$it->recursed && $it->hasChildren()) {
					$it->recursed = true;
					$sub = $it->getChildren();
					$sub->recursed = false;
					$sub->rewind();
					if ($sub->hasMore()) {
						$this->ait[$this->count++] = $sub;
						if (!is_a($sub, 'RecursiveIterator')) {
							throw new Exception(get_class($sub).'::getChildren() must return an object that implements RecursiveIterator');
						} 
						return;
					}
					unset($sub);
				}
				$it->next();
				$it->recursed = false;
				if ($it->hasMore()) {
					return;
				}
				$it->recursed = false;
			}
			if ($this->count <= 1) {
				return;
			}
			unset($this->ait[--$this->count]);
			$it = $this->ait[$this->count-1];
		}
	}
	
	function getCurrentIterator() {
		return $this->ait[$this->count-1];
	}
}

?>