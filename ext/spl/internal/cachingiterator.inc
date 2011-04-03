<?php

/** @file cachingiterator.inc
 * @ingroup SPL
 * @brief class CachingIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Cached iteration over another Iterator
 * @author  Marcus Boerger
 * @version 1.2
 * @since PHP 5.0
 *
 * This iterator wrapper does a one ahead iteration. This way it knows whether
 * the inner iterator has one more element.
 *
 * @note If you want to convert the elements into strings and the inner 
 *       Iterator is an internal Iterator then you need to provide the 
 *       flag CALL_TOSTRING to do the conversion when the actual element
 *       is being fetched. Otherwise the conversion would happen with the
 *       already changed iterator. If you do not need this then it you should
 *       omit this flag because it costs unneccessary work and time.
 */
class CachingIterator implements OuterIterator
{
	const CALL_TOSTRING        = 0x00000001;
	const CATCH_GET_CHILD      = 0x00000002;
	const TOSTRING_USE_KEY     = 0x00000010;
	const TOSTRING_USE_CURRENT = 0x00000020;

	private $it;
	private $current;
	private $key;
	private $valid;
	private $strValue;

	/** Construct from another iterator
	 *
	 * @param it    Iterator to cache
	 * @param flags Bitmask: 
	 *              - CALL_TOSTRING  (whether to call __toString() for every element)
	 */
	function __construct(Iterator $it, $flags = self::CALL_TOSTRING)
	{
		if ((($flags & self::CALL_TOSTRING) && ($flags & (self::TOSTRING_USE_KEY|self::TOSTRING_USE_CURRENT)))
		|| ((flags & (self::CIT_TOSTRING_USE_KEY|self::CIT_TOSTRING_USE_CURRENT)) == (self::CIT_TOSTRING_USE_KEY|self::CIT_TOSTRING_USE_CURRENT)))
		{
			throw new InvalidArgumentException('Flags must contain only one of CIT_CALL_TOSTRING, CIT_TOSTRING_USE_KEY, CIT_TOSTRING_USE_CURRENT');
		}
		$this->it = $it;
		$this->flags = $flags & (0x0000FFFF);
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
			if ($this->flags & self::CALL_TOSTRING) {
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
	
	/** @return whether the iterator is valid
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
	 *
	 * @param func    Name of method to invoke
	 * @param params  Array of parameters to pass to method
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->it, $func), $params);
	}
	
	/** @return the string represenatation that was generated for the current 
	 *          element
	 * @throw exception when CALL_TOSTRING was not specified in constructor
	 */
	function __toString()
	{
		if ($this->flags & self::TOSTRING_USE_KEY)
		{
			return $this->key;
		}
		else if ($this->flags & self::TOSTRING_USE_CURRENT)
		{
			return $this->current;
		}
		if (!$this->flags & self::CALL_TOSTRING)
		{
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
