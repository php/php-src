<?php

/** \brief seekable iterator
 *
 * Turns a normal iterator ino a seekable iterator. When there is a way
 * to seek on an iterator LimitIterator can use this to efficiently rewind
 * to offset.
 */
interface SeekableIterator implements Iterator
{
	/** Seek to an absolute position
	 *
	 * \param $index position to seek to
	 * \return void
	 *
	 * \note The method should throw an exception if it is not possible to
	 *       seek to the given position.
	 */
	function seek($index);
/*		$this->rewind();
		$position = 0;
		while($position < $index && $this->valid()) {
			$this->next();
			$position++;
		}
	}*/
}

?>