<?php

define('CIT_GET_STR_VALUE', 1);
define('CIT_CATCH_GET_CHILD', 2);

class CachingIterator
{
	protected $it;
	protected $current;
	protected $key;
	protected $more;
	protected $strValue;
	protected $getStrVal;

	function __construct(Iterator $it, $flags = CIT_GET_STR_VALUE)
	{
		$this->it = $it;
		$this->flags = $flags & (CIT_GET_STR_VALUE|CIT_CATCH_GET_CHILD);
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
			if ($this->flags & CIT_GET_STR_VALUE) {
				if (is_object($this->current)) {
					$this->strValue = $this->current->__toString();
				} else {
					$this->strValue = (string)$this->current;
				}
			}
		} else {
			$this->current = NULL;
			$this->key = NULL;
			$this->strValue = NULL;
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
		if (!$this->flags & CIT_GET_STR_VALUE) {
			throw new exception('CachingIterator does not fetch string value (see CachingIterator::__construct)');
		}
		return $this->strValue;
	}
}

?>