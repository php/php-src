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
	const USE_KEY     = 0x00000001; /**< If present in $flags the the key is 
	                                     used rather then the current value. */

	const MATCH       = 0; /**< Mode: Executed a plain match only      */
	const GET_MATCH   = 1; /**< Mode: Return the first matche (if any) */
	const ALL_MATCHES = 2; /**< Mode: Return all matches (if any)      */
	const SPLIT       = 3; /**< Mode: Return the split values (if any) */
	const REPLACE     = 4; /**< Mode: Replace the input key or current */
	
	private $regex;     /**< the regular expression to match against */
	private $flags;     /**< special flags (self::USE_KEY) */
	private $mode;      /**< operation mode (one of self::MATCH, 
	                         self::GET_MATCH, self::ALL_MATCHES, self::SPLIT) */
	private $preg_flags;/**< PREG_* flags, see preg_match(), preg_match_all(), 
	                         preg_split() */ 
	private $current;   /**< the value used for current() */

	/**
	 * Constructs a regular expression filter around an iterator whose 
	 * elemnts or keys are strings.
	 *
	 * @param it          inner iterator
	 * @param regex       the regular expression to match
	 * @param flags       special flags (self::USE_KEY)
	 * @param mode        operation mode (one of self::MATCH, self::GET_MATCH, 
	 *                    self::ALL_MATCHES, self::SPLIT)
	 * @param preg_flags  global PREG_* flags, see preg_match(), 
	 *                    preg_match_all(), preg_split()
	 */
	function __construct(Iterator $it, $regex, $flags = 0, $mode = 0, $preg_flags = 0) {
		parent::__construct($it);
		$this->regex = $regex;
		$this->flags = $flags;
		$this->mode = $mode;
		$this->preg_flags = $preg_flags;
	}

	/**
	 * Match current or key against regular expression using mode, flags and
	 * preg_flags.
	 *
	 * @return whether this is a match
	 *
	 * @warning never call this twice for the same state
	 */
	function accept()
	{
		$matches       = array();
		$this->current = parent::current();
		/* note that we use $this->current, rather than calling parent::current() */
		$subject = ($this->flags & self::USE_KEY) ? parent::key() : $this->current;
		switch($this->mode)
		{
			case self::MATCH:
				return preg_match($this->regex, $subject, $matches, $this->preg_flags);

			case self::GET_MATCH:
				$this->current = array();
				return preg_match($this->regex, $subject, $this->current, $this->preg_flags) > 0;

			case self::ALL_MATCHES:
				$this->current = array();
				return preg_match_all($this->regex, $subject, $this->current, $this->preg_flags) > 0;

			case self::SPLIT:
				$this->current = array();
				preg_split($this->regex, $subject, $this->current, $this->preg_flags) > 1;
		}
	}

	/** @return the current value after accept has been called
	 */
	function current()
	{
		return $this->current;
	}
}

?>