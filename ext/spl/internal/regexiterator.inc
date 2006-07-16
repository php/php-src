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
 * @version 1.1
 * @since PHP 5.1
 *
 * This filter iterator assumes that the inner iterator 
 */
class RegexIterator implements FilterIterator
{
	const USE_KEY     = 0x00000001;

	const MATCH       = 0;
	const GET_MATCH   = 1;
	const ALL_MATCHES = 2;
	const SPLIT       = 3;
	
	private $regex;     /**< the regular expression to match against */
	private $flags;     /**< special flags (USE_KEY) */
	private $mode;      /**< operation mode (MATCH, GET_MATCH, ALL_MATCHES, SPLIT) */
	private $preg_flags;/**< PREG_* flags, see preg_match(), preg_match_all(), preg_split() */ 
	private $current;   /**< the value used for current() */

	/**
	 * Constructs a regular expression filter around an iterator whose 
	 * elemnts or keys are strings.
	 *
	 * @param it     Object that implements at least
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