<?php
/** @file multipleiterator.inc
 * @ingroup SPL
 * @brief class MultipleIterator
 * @author  Johannes Schlueter
 * @author  Marcus Boerger
 * @date    2008 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   Iterator that iterates over several iterators one after the other
 * @author  Johannes Schlueter
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.3
 */
class MultipleIterator implements Iterator
{
	/** Inner Iterators */
	private $iterators;

	/** Flags: const MIT_* */
	private $flags;

	/** do not require all sub iterators to be valid in iteration */
	const MIT_NEED_ANY = 0;

	/** require all sub iterators to be valid in iteration */
	const MIT_NEED_ALL  = 1;

	/** keys are created from sub iterators position */
	const MIT_KEYS_NUMERIC  = 0;

	/** keys are created from sub iterators associated infromation */
	const MIT_KEYS_ASSOC  = 2;

	/** Construct a new empty MultipleIterator
	* @param flags MIT_* flags
	*/
	public function __construct($flags = self::MIT_NEED_ALL|self::MIT_KEYS_NUMERIC)
	{
		$this->iterators = new SplObjectStorage();
		$this->flags = $flags;
	}

	/** @return current flags MIT_* */
	public function getFlags()
	{
		return $this->flags;
	}

	/** @param $flags new flags. */
	public function setFlags($flags)
	{
		$this->flags = $flags;
	}

	/** @param $iter new Iterator to attach.
	* @param $inf associative info forIteraotr, must be NULL, integer or string
	*
	* @throws IllegalValueException if a inf is none of NULL, integer or string
	* @throws IllegalValueException if a inf is already an associated info
	*/
	public function attachIterator(Iterator $iter, $inf = NULL)
	{

		if (!is_null($inf))
		{
			if (!is_int($inf) && !is_string($inf))
			{
				throw new IllegalValueException('Inf must be NULL, integer or string');
			}
			foreach($this->iterators as $iter)
			{
				if ($inf == $this->iterators->getInfo())
				{
					throw new IllegalValueException('Key duplication error');
				}
			}
		}
		$this->iterators->attach($iter, $inf);
	}

	/** @param $iter attached Iterator that should be detached. */
	public function detachIterator(Iterator $iter)
	{
		$this->iterators->detach($iter);
	}

	/** @param $iter Iterator to check
	* @return whether $iter is attached or not
	*/
	public function containsIterator(Iterator $iter)
	{
		return $this->iterator->contains($iter);
	}

	/** @return number of attached Iterator instances. */
	public function countIterators()
	{
		return $this->iterators->count();
	}

	/** Rewind all attached Iterator instances. */
	public function rewind()
	{
		foreach($this->iterators as $iter)
		{
			$iter->rewind();
		}
	}

	/**
	* @return whether all or one sub iterator is valid depending on flags.
	* In mode MIT_NEED_ALL we expect all sub iterators to be valid and
	* return flase on the first non valid one. If that flag is not set we
	* return true on the first valid sub iterator found. If no Iterator
	* is attached, we always return false.
	*/
	public function valid()
	{
		if (!sizeof($this->iterators)) {
			return false;
		}
		// The following code is an optimized version that executes as few
		// valid() calls as necessary and that only checks the flags once.
		$expect = $this->flags & self::MIT_NEED_ALL ? true : false;
		foreach($this->iterators as $iter)
		{
			if ($expect != $iter->valid())
			{
				return !$expect;
			}
		}
		return $expect;
	}

	/** Move all attached Iterator instances forward. That is invoke
	* their next() method regardless of their state.
	*/
	public function next()
	{
		foreach($this->iterators as $iter)
		{
			$iter->next();
		}
	}

	/** @return false if no sub Iterator is attached and an array of
	* all registered Iterator instances current() result.
	* @throws RuntimeException      if mode MIT_NEED_ALL is set and at least one
	*                               attached Iterator is not valid().
	* @throws IllegalValueException if a key is NULL and MIT_KEYS_ASSOC is set.
	*/
	public function current()
	{
		if (!sizeof($this->iterators))
		{
			return false;
		}
		$retval = array();
		foreach($this->iterators as $iter)
		{
			if ($it->valid())
			{
				if ($this->flags & self::MIT_KEYS_ASSOC)
				{
					$key = $this->iterators->getInfo();
					if (is_null($key))
					{
						throw new IllegalValueException('Sub-Iterator is associated with NULL');
					}
					$retval[$key] = $iter->current();
				}
				else
				{
					$retval[] = $iter->current();
				}
			}
			else if ($this->flags & self::MIT_NEED_ALL)
			{
				throw new RuntimeException('Called current() with non valid sub iterator');
			}
			else
			{
				$retval[] = NULL;
			}
		}
		return $retval;
	}

	/** @return false if no sub Iterator is attached and an array of
	* all registered Iterator instances key() result.
	* @throws LogicException if mode MIT_NEED_ALL is set and at least one
	*         attached Iterator is not valid().
	*/
	public function key()
	{
		if (!sizeof($this->iterators))
		{
			return false;
		}
		$retval = array();
		foreach($this->iterators as $iter)
		{
			if ($it->valid())
			{
				$retval[] = $iter->key();
			}
			else if ($this->flags & self::MIT_NEED_ALL)
			{
				throw new LogicException('Called key() with non valid sub iterator');
			}
			else
			{
				$retval[] = NULL;
			}
		}
		return $retval;
	}
}
