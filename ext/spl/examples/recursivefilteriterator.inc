<?php

/** @file recursivefilteriterator.inc
 * @ingroup Examples
 * @brief class RecursiveFilterIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   A recursive Filter
 * @author  Marcus Boerger
 * @version 1.0
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
abstract class RecursiveFilterIterator extends FilterIterator implements RecursiveIterator
{
	/*! The constructor takes a RecursiveIterator
	 */
	function __construct(RecursiveIterator $it)
	{
		$this->ref = new ReflectionClass($this);
		parent::__construct($it);
	}

	/*! return whether the inner iterator has children
	 */
	function hasChildren()
	{
		return $this->getInnerIterator()->hasChildren();
	}

	/*! \return children as instance of derived RecursiveFilterIterator class
	 *
	 *  \see RecursiveFilterIterator
	 */
	function getChildren()
	{
		return $this->ref->newInstance($this->getInnerIterator()->getChildren());
	}
	
	private $ref;
}

?>