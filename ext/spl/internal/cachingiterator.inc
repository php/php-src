<?php

define('CIT_CALL_TOSTRING', 1);
define('CIT_CATCH_GET_CHILD', 2);

class CachingIterator
{
	protected $it;
	protected $current;
	protected $key;
	protected $valid;
	protected $strValue;

	function __construct(Iterator $it, $flags = CIT_CALL_TOSTRING)
	{
		$this->it = $it;
		$this->flags = $flags & (CIT_CALL_TOSTRING|CIT_CATCH_GET_CHILD);
		$this->next();
	}

	function rewind()
	{
		$this->it->rewind();
		$this->next();
	}
	
	function next()
	{
		if ($this->valid = $this->it->valid()) {
			$this->current = $this->it->current();
			$this->key = $this->it->key();
			if ($this->flags & CIT_CALL_TOSTRING) {
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
	
	function valid()
	{
		return $this->valid;
	}

	function hasNext()
	{
		return $this->it->valid();
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
		if (!$this->flags & CIT_CALL_TOSTRING) {
			throw new exception('CachingIterator does not fetch string value (see CachingIterator::__construct)');
		}
		return $this->strValue;
	}
}

?>