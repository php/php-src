<?php

/** @file recursiveregexiterator.inc
 * @ingroup SPL
 * @brief class RegexIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
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
	 * @param it          inner iterator
	 * @param regex       the regular expression to match
	 * @param mode        operation mode (one of self::MATCH, self::GET_MATCH,
	 *                    self::ALL_MATCHES, self::SPLIT)
	 * @param flags       special flags (self::USE_KEY)
	 * @param preg_flags  global PREG_* flags, see preg_match(),
	 *                    preg_match_all(), preg_split()
	 */
	function __construct(RecursiveIterator $it, $regex, $mode = 0, $flags = 0, $preg_flags = 0) {
		parent::__construct($it, $regex, $mode, $flags, $preg_flags);
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
