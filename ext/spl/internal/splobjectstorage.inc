<?php

/** @file splobjectstorage.inc
 * @ingroup SPL
 * @brief class SplObjectStorage
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Object storage
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1.2
 *
 * This container allows to store objects uniquly without the need to compare
 * them one by one. This is only possible internally. The code represenation
 * here therefore has a complexity of O(n) while the actual implementation has
 * complexity O(1).
 */
class SplObjectStorage implements Iterator, Countable
{
	private $storage = array();
	private $index = 0;

	/** Rewind to top iterator as set in constructor
	 */
	function rewind()
	{
		rewind($this->storage);
	}
	
	/** @return whether iterator is valid
	 */
	function valid()
	{
		return key($this->storage) !== false;
	}
	
	/** @return current key
	 */
	function key()
	{
		return $this->index;
	}
	
	/** @return current object
	 */
	function current()
	{
		return current($this->storage);
	}
	
	/** Forward to next element
	 */
	function next()
	{
		next($this->storage);
		$this->index++;
	}

	/** @return number of objects in storage
	 */
	function count()
	{
		return count($this->storage);
	}

	/** @param obj object to look for
	 * @return whether $obj is contained in storage
	  */
	function contains($obj)
	{
		if (is_object($obj))
		{
			foreach($this->storage as $object)
			{
				if ($object === $obj)
				{
					return true;
				}
			}
		}
		return false;
	}

	/** @param $obj new object to attach to storage if not yet contained
	 */
	function attach($obj)
	{
		if (is_object($obj) && !$this->contains($obj))
		{
			$this->storage[] = $obj;
		}
	}

	/** @param $obj object to remove from storage
	 */
	function detach($obj)
	{
		if (is_object($obj))
		{
			foreach($this->storage as $idx => $object)
			{
				if ($object === $obj)
				{
					unset($this->storage[$idx]);
					$this->rewind();
					return;
				}
			}
		}
	}
}

?>
