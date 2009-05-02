<?php

/** @file recursivearrayiterator.inc
 * @ingroup Examples
 * @brief class RecursiveArrayIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   A recursive array iterator
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1
 *
 * Passes the RecursiveIterator interface to the inner Iterator and provides
 * the same functionality as FilterIterator. This allows you to skip parents
 * and all their childs before loading them all. You need to care about
 * function getChildren() because it may not always suit your needs. The 
 * builtin behavior uses reflection to return a new instance of the exact same
 * class it is called from. That is you extend RecursiveFilterIterator and
 * getChildren() will create instance of that class. The problem is that doing
 * this does not transport any state or control information of your accept()
 * implementation to the new instance. To overcome this problem you might 
 * need to overwrite getChildren(), call this implementation and pass the
 * control vaules manually.
 */
class RecursiveArrayIterator extends ArrayIterator implements RecursiveIterator
{
	/** @return whether the current element has children
	 */
	function hasChildren()
	{
		return is_array($this->current());
	}

	/** @return an iterator for the current elements children
	 *
	 * @note the returned iterator will be of the same class as $this
	 */
	function getChildren()
	{
		if ($this->current() instanceof self)
		{
			return $this->current();
		}
		if (empty($this->ref))
		{
			$this->ref = new ReflectionClass($this);
		}
		return $this->ref->newInstance($this->current());
	}
	
	private $ref;
}

?>