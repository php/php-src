<?php

/** @file keyfilter.inc
 * @ingroup Examples
 * @brief class KeyFilter
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   Regular expression filter for string iterators
 * @author  Marcus Boerger
 * @version 1.1
 *
 * Instances of this class act as a filter around iterators whose elements
 * are strings. In other words you can put an iterator into the constructor
 * and the instance will only return elements which match the given regular 
 * expression.
 */
class KeyFilter extends FilterIterator
{
	/** @internal regular exoression used as filter */
	private $regex;

	/**
	 * Constructs a filter around an iterator whose elemnts are strings.
	 * If the given iterator is of type spl_sequence then its rewind()
	 * method is called.
	 *
	 * @param it     Object that implements at least spl_forward
	 * @param regex  Regular expression used as a filter.
	 */
	function __construct(Iterator $it, $regex)
	{
		parent::__construct($it);
		$this->regex = $regex;
	}

	/** \return whether the current key mathes the regular expression 
	 */
	function accept()
	{
		return ereg($this->regex, $this->getInnerIterator()->key());
	}
	
	/** @return regular expression used as filter
	 */
	function getRegex()
	{
		return $this->regex;
	}

	/**
	 * hidden __clone
	 */
	protected function __clone()
	{
		// disallow clone 
	}
}

?>