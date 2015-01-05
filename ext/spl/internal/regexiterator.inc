<?php

/** @file regexiterator.inc
 * @ingroup SPL
 * @brief class RegexIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
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
class RegexIterator extends FilterIterator
{
	const USE_KEY     = 0x00000001; /**< If present in $flags the key is 
	                                     used rather then the current value. */

	const MATCH       = 0; /**< Mode: Executed a plain match only      */
	const GET_MATCH   = 1; /**< Mode: Return the first matche (if any) */
	const ALL_MATCHES = 2; /**< Mode: Return all matches (if any)      */
	const SPLIT       = 3; /**< Mode: Return the split values (if any) */
	const REPLACE     = 4; /**< Mode: Replace the input key or current */
	
	private $regex;     /**< the regular expression to match against */
	private $mode;      /**< operation mode (one of self::MATCH, 
	                         self::GET_MATCH, self::ALL_MATCHES, self::SPLIT) */
	private $flags;     /**< special flags (self::USE_KEY) */
	private $preg_flags;/**< PREG_* flags, see preg_match(), preg_match_all(), 
	                         preg_split() */ 
	private $key;       /**< the value used for key() */
	private $current;   /**< the value used for current() */

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
	function __construct(Iterator $it, $regex, $mode = 0, $flags = 0, $preg_flags = 0) {
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
		$this->key     = parent::key();
		$this->current = parent::current();
		/* note that we use $this->current, rather than calling parent::current() */
		$subject = ($this->flags & self::USE_KEY) ? $this->key : $this->current;
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

			case self::REPLACE:
				$this->current = array();
				$result = preg_replace($this->regex, $this->replacement, $subject);
				if ($this->flags & self::USE_KEY)
				{
					$this->key = $result;
				}
				else
				{
					$this->current = $result;
				}
		}
	}

	/** @return the key after accept has been called
	 */
	function key()
	{
		return $this->key;
	}

	/** @return the current value after accept has been called
	 */
	function current()
	{
		return $this->current;
	}

	/** @return current operation mode
	 */
	function getMode()
	{
		return $this->mode;
	}

	/** @param mode new operaion mode
	 */
	function setMode($mode)
	{
		$this->mode = $mode;
	}

	/** @return current operation flags
	 */
	function getFlags()
	{
		return $this->flags;
	}

	/** @param flags new operaion flags
	 */
	function setFlags($flags)
	{
		$this->flags = $flags;
	}

	/** @return current PREG flags
	 */
	function getPregFlags()
	{
		return $this->preg_flags;
	}

	/** @param preg_flags new PREG flags
	 */
	function setPregFlags($preg_flags)
	{
		$this->preg_flags = $preg_flags;
	}

	/** @return current regular expression
	*/
	function getRegex()
	{
		return $this->regex;
	}
}

?>
