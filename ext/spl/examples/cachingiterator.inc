<?php

class CachingIterator
{
	protected $it;
	protected $current;
	protected $key;
	protected $more;
	protected $strvalue;

	function __construct(Iterator $it)
	{
		$this->it = $it;
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
			$this->strvalue = (string)$this->current;
		} else {
			$this->current = NULL;
			$this->key = NULL;
			$this->strvalue = '';
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
		return $this->strvalue;
	}
}

?>