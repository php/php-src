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

	function __construct(RecursiveIterator $it)
	{
		$this->ait[0] = $it;
	}


	function rewind()
	{
		while ($this->count) {
			unset($this->ait[$this->count--]);
		}
		$this->ait[0]->rewind();
		$this->ait[0]->recursed = false;
	}
	
	function valid()
	{
		$count = $this->count;
		while ($count) {
			$it = $this->ait[$count];
			if ($it->valid()) {
				return true;
			}
			$count--;
		}
		return false;
	}
	
	function key()
	{
		$it = $this->ait[$this->count];
		return $it->key();
	}
	
	function current()
	{
		$it = $this->ait[$this->count];
		return $it->current();
	}
	
	function next()
	{
		while ($this->count) {
			$it = $this->ait[$this->count];
			if ($it->valid()) {
				if (!$it->recursed && $it->hasChildren()) {
					$it->recursed = true;
					$sub = $it->getChildren();
					$sub->recursed = false;
					$sub->rewind();
					if ($sub->valid()) {
						$this->ait[++$this->count] = $sub;
						if (!$sub instanceof RecursiveIterator) {
							throw new Exception(get_class($sub).'::getChildren() must return an object that implements RecursiveIterator');
						} 
						return;
					}
					unset($sub);
				}
				$it->next();
				$it->recursed = false;
				if ($it->valid()) {
					return;
				}
				$it->recursed = false;
			}
			if ($this->count) {
				unset($this->ait[$this->count--]);
				$it = $this->ait[$this->count];
			}
		}
	}
	
	function getSubIterator($level = NULL)
	{
		if (is_null($level)) {
			$level = $this->count;
		}
		return @$this->ait[$level];
	}

	function getDepth()
	{
		return $this->level;
	}
}

?>