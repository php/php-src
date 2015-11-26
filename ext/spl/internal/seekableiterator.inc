<?php

/** @file seekableiterator.inc
 * @ingroup SPL
 * @brief class SeekableIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @brief seekable iterator
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.0
 *
 * Turns a normal iterator ino a seekable iterator. When there is a way
 * to seek on an iterator LimitIterator can use this to efficiently rewind
 * to offset.
 */
interface SeekableIterator extends Iterator
{
	/** Seek to an absolute position
	 *
	 * \param $index position to seek to
	 * \return void
	 *
	 * The method should throw an exception if it is not possible to seek to 
	 * the given position. Typically this exception should be of type 
	 * OutOfBoundsException.
	 \code
	function seek($index);
		$this->rewind();
		$position = 0;
		while($position < $index && $this->valid()) {
			$this->next();
			$position++;
		}
		if (!$this->valid()) {
			throw new OutOfBoundsException('Invalid seek position');
		}
	}
	 \endcode
	 */
	function seek($index);
}

?>