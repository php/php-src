<?php

/** @file regexiterator.inc
 * @ingroup SPL
 * @brief class RegexIterator
 * @author  Marcus Boerger
 * @date    2003 - 2006
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Regular expression filter for iterators
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1
 *
 * This filter iterator assumes that the inner iterator 
 */
class RegexIterator implements FilterIterator
{
	const USE_KEY     = 0x00000001;
	
	private $regex;     /**< the regular expression to match against */
	private $flags;     /**< special flags (USE_KEY) */

	/**
	 * Constructs a regular expression filter around an iterator whose 
	 * elemnts or keys are strings.
	 *
	 * @param it     Object that implements at least
	 */
	function __construct(Iterator $it, $regex, $flags = 0) {
		parent::__construct($it);
		$this->regex = $regex;
		$this->flags = $flags;
	}

	/**
	 * Match current or key against regular expression.
	 *
	 * @return whether this is a match
	 */
	function accept()
	{
		$subject = ($this->flags & self::USE_KEY) ? parent::key() : parent::current();
		return preg_match($this->regex, $subject);
	}
}

?>