<?php

/** @file recursiveregexiterator.inc
 * @ingroup SPL
 * @brief class RegexIterator
 * @author  Marcus Boerger
 * @date    2003 - 2006
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Recursive regular expression filter for iterators
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1
 *
 * This filter iterator assumes that the inner iterator 
 */
class RecursiveRegexIterator extends RegexIterator implements RecursiveIterator
{
	/**
	 * Constructs a regular expression filter around an iterator whose 
	 * elemnts or keys are strings.
	 *
	 * @param it     Object that implements at least
	 */
	function __construct(RecursiveIterator $it, $regex, $flags = 0, $mode = 0, $preg_flags = 0) {
		parent::__construct($it, $regex, $flags, $mode, $preg_flags);
	}

	/** @return whether the current element has children
	 */
	function hasChildren()
	{
		return $this->getInnerIterator()->hasChildren();
	}

	/** @return an iterator for the current elements children
	 *
	 * @note the returned iterator will be of the same class as $this
	 */
	function getChildren()
	{
		if (empty($this->ref))
		{
			$this->ref = new ReflectionClass($this);
		}
		return $this->ref->newInstance($this->getInnerIterator()->getChildren());
	}
	
	private $ref;
}

?>