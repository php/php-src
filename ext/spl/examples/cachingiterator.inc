<?php

class CachingIterator
{
	protected $it;
	protected $current;
	protected $key;
	protected $more;
	protected $strValue;
	protected $getStrVal;

	function __construct(Iterator $it, $getStrVal = true)
	{
		$this->it = $it;
		$this->getStrVal = (boolean)$getStrVal;
	}

	function rewind()
	{
		$this->it->rewind();
		$this->next();
	}
	
	function next()
	{
		if ($this->more = $this->it->hasMore()) {
			$this->current = $this->it->current();
			$this->key = $this->it->key();
			if ($this->getStrVal) {
				if (is_object($this->current)) {
					$this->strValue = $this->current->__toString();
				} else {
					$this->strValue = (string)$this->current;
				}
			}
		} else {
			$this->current = NULL;
			$this->key = NULL;
			$this->strValue = '';
		}
		$this->it->next();
	}
	
	function hasMore()
	{
		return $this->more;
	}

	function hasNext()
	{
		return $this->it->hasMore();
	}
	
	function current()
	{
		return $this->current;
	}

	function key()
	{
		return $this->key;
	}

	function __call($func, $params)
	{
		return call_user_func_array(array($this->it, $func), $params);
	}
	
	function __toString()
	{
		if (!$this->getStrVal) {
			throw new exception('CachingIterator does not fetch string value (see CachingIterator::__construct)');
		}
		return $this->strValue;
	}
}

?>