<?php

/** @file callbackfilteriterator.inc
 * @ingroup Examples
 * @brief class CallbackFilterIterator
 * @author  Marcus Boerger
 * @author  Kevin McArthur
 * @date    2006 - 2006
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   A non abstract FiletrIterator that uses a callback foreach element
 * @author  Marcus Boerger
 * @author  Kevin McArthur
 * @version 1.0
 */
class CallbackFilterIterator extends FilterIterator
{
	const USE_FALSE = 0;  /**< mode: accept no elements, no callback */
	const USE_TRUE  = 1;  /**< mode: accept all elements, no callback */
	const USE_VALUE = 2;  /**< mode: pass value to callback */
	const USE_KEY   = 3;  /**< mode: pass key to callback */
	const USE_BOTH  = 4;  /**< mode: pass value and key to callback */

	const REPLACE   = 0x00000001; /**< flag: pass key/value by reference */

	private $callback; /**< callback to use */
	private $mode;     /**< mode any of USE_VALUE, USE_KEY, USE_BOTH */
	private $flags;    /**< flags (REPLACE) */
	private $key;      /**< key value */
	private $current;  /**< current value */

	/** Construct a CallbackFilterIterator
	 *
	 * @param it        inner iterator (iterator to filter)
	 * @param callback  callback function
	 * @param mode      any of USE_VALUE, USE_KEY, USE_BOTH
	 * @param flags     any of 0, REPLACE
	 */
	public function __construct(Iterator $it, $callback, $mode = self::USE_VALUE, $flags = 0)
	{
		parent::__construct($it);
		$this->callback = $callback;
		$this->mode     = $mode;
		$this->flags    = $flags;
	}

	/** Call the filter callback
	 * @return result of filter callback
	 */
	public function accept()
	{
		$this->key     = parent::key();
		$this->current = parent::current();

		switch($this->mode) {
		default:
		case self::USE_FALSE;
			return false;
		case self::USE_TRUE:
			return true;
		case self::USE_VALUE:
			if($this->flags & self::REPLACE) {
				return (bool) call_user_func($this->callback, &$this->current);
			} else {
				return (bool) call_user_func($this->callback, $this->current);
			}
		case self::USE_KEY:
			if($this->flags & self::REPLACE) {
				return (bool) call_user_func($this->callback, &$this->key);
			} else {
				return (bool) call_user_func($this->callback, $this->key);
			}
		case SELF::USE_BOTH:
			if($this->flags & self::REPLACE) {
				return (bool) call_user_func($this->callback, &$this->key, &$this->current);
			} else {
				return (bool) call_user_func($this->callback, $this->key, $this->current);
			}
		}
	}

	/** @return current key value */
	function key()
	{
		return $this->key;
	}

	/** @return current value */
	function current()
	{
		return $this->current;
	}

	/** @return operation mode */
	function getMode()
	{
		return $this->mode;
	}

	/** @param $mode set new mode, @see mode */
	function setMode($mode)
	{
		$this->mode = $mode;
	}

	/** @return operation flags */
	function getFlags()
	{
		return $this->flags;
	}

	/** @param $flags set new flags, @see flags */
	function setFlags($flags)
	{
		$this->flags = $flags;
	}
}

?>