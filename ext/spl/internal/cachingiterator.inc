<?php

/** @file cachingiterator.inc
 * @ingroup Internal
 * @brief class CachingIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

define('CIT_CALL_TOSTRING', 1);
define('CIT_CATCH_GET_CHILD', 2);

/**
 * @brief   Cached Iteration over another Iterator
 * @author  Marcus Boerger
 * @version 1.1
 *
 */
class CachingIterator implements OuterIterator
{
	protected $it;
	protected $current;
	protected $key;
	protected $valid;
	protected $strValue;

	/** Construct from another iterator
	 *
	 * @param it    Iterator to cache
	 * @param flags Bitmask: 
	 *              - CIT_CALL_TOSTRING  (whether to call __toString() for every element)
	 */
	function __construct(Iterator $it, $flags = CIT_CALL_TOSTRING)
	{
		$this->it = $it;
		$this->flags = $flags & (CIT_CALL_TOSTRING|CIT_CATCH_GET_CHILD);
		$this->next();
	}

	/** Rewind the Iterator
	 */
	function rewind()
	{
		$this->it->rewind();
		$this->next();
	}
	
	/** Forward to the next element
	 */
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
	
	/** @return whether teh iterator is valid
	 */
	function valid()
	{
		return $this->valid;
	}

	/** @return whether there is one more element
	 */
	function hasNext()
	{
		return $this->it->valid();
	}
	
	/** @return the current element
	 */
	function current()
	{
		return $this->current;
	}

	/** @return the current key
	 */
	function key()
	{
		return $this->key;
	}

	/** Aggregate the inner iterator
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->it, $func), $params);
	}
	
	/** @return the string represenatation that was generated for the current 
	 *          element
	 * @throw exception when CIT_CALL_TOSTRING was not specified in constructor
	 */
	function __toString()
	{
		if (!$this->flags & CIT_CALL_TOSTRING) {
			throw new exception('CachingIterator does not fetch string value (see CachingIterator::__construct)');
		}
		return $this->strValue;
	}
	
	/**
	 * @return The inner iterator
	 */	
	function getInnerIterator()
	{
		return $this->it;
	}
}

?>