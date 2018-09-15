<?php

/** @file norewinditerator.inc
 * @ingroup SPL
 * @brief class NoRewindIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   An Iterator wrapper that doesn't call rewind
 * @author  Marcus Boerger
 * @version 1.1
 * @since PHP 5.1
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
