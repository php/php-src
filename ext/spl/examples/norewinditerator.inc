<?php

/** @file norewinditerator.inc
 * @ingroup Examples
 * @brief class NoRewindIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/** @ingroup Examples
 * @brief   An Iterator that doesn't call rewind
 * @author  Marcus Boerger
 * @version 1.1
 *
 */
class NoRewindIterator extends IteratorIterator
{
	/** Simply prevent execution of inner iterators rewind().
	 */
	function rewind()
	{
		// nothing to do
	}
}

?>