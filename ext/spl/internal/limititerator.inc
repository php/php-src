<?php

/** @file limititerator.inc
 * @ingroup Internal
 * @brief class LimitIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Limited Iteration over another Iterator
 * @author  Marcus Boerger
 * @version 1.1
 *
 */
class LimitIterator implements OuterIterator
{
	protected $it;
	protected $offset;
	protected $count;
	private $pos;

	/** Construct
	 *
	 * @param it     Iterator to limit
	 * @param offset Offset to first element
	 * @param count  Maximum number of elements to show or NULL for all
	 */
	function __construct(Iterator $it, $offset = 0, $count = -1)
	{
		if ($offset < 0) {
			throw new exception('Parameter offset must be > 0');
		}
		if ($count < 0 && $count != -1) {
			throw new exception('Parameter count must either be -1 or a value greater than or equal to 0');
		}
		$this->it     = $it;
		$this->offset = $offset;
		$this->count  = $count;
		$this->pos    = 0;
	}
	
	/** Seek to specified position
	 * @param position offset to seek to (relative to beginning not offset
	 *                 specified in constructor).
	 * @throw exception when position is invalid
	 */
	function seek($position) {
		if ($position < $this->offset) {
			throw new exception('Cannot seek to '.$position.' which is below offset '.$this->offset);
		}
		if ($position > $this->offset + $this->count && $this->count != -1) {
			throw new exception('Cannot seek to '.$position.' which is behind offset '.$this->offset.' plus count '.$this->count);
		}
		if ($this->it instanceof SeekableIterator) {
			$this->it->seek($position);
			$this->pos = $position;
		} else {
			while($this->pos < $position && $this->it->valid()) {
				$this->next();
			}
		}
	}

    /** Rewind to offset specified in constructor
     */
	function rewind()
	{
		$this->it->rewind();
		$this->pos = 0;
		$this->seek($this->offset);
	}
	
	/** @return whether iterator is valid
	 */
	function valid() {
		return ($this->count == -1 || $this->pos < $this->offset + $this->count)
			 && $this->it->valid();
	}
	
	/** @return current key
	 */
	function key() {
		return $this->it->key();
	}

	/** @return current element
	 */
	function current() {
		return $this->it->current();
	}

	/** Forward to nect element
	 */
	function next() {
		$this->it->next();
		$this->pos++;
	}

	/** @return current position relative to zero (not to offset specified in 
	 *          constructor).
	 */
	function getPosition() {
		return $this->pos;
	}

	/**
	 * @return The inner iterator
	 */	
	function getInnerIterator()
	{
		return $this->it;
	}
}

?>