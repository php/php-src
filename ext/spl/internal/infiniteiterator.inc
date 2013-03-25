<?php

/** @file infiniteiterator.inc
 * @ingroup SPL
 * @brief class InfiniteIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief   An infinite Iterator
 * @author  Marcus Boerger
 * @version 1.1
 * @since PHP 5.1
 *
 * This Iterator takes another Iterator and infinitvely iterates it by
 * rewinding it when its end is reached.
 *
 * \note Even an InfiniteIterator stops if its inner Iterator is empty.
 *
 \verbatim
 $it       = new ArrayIterator(array(1,2,3));
 $infinite = new InfiniteIterator($it);
 $limit    = new LimitIterator($infinite, 0, 5);
 foreach($limit as $val=>$key)
 {
 	echo "$val=>$key\n";
 }
 \endverbatim
 */
class InfiniteIterator extends IteratorIterator
{
	/** Move the inner Iterator forward to its next element or rewind it.
	 * @return void
	 */
	function next()
	{
		$this->getInnerIterator()->next();
		if (!$this->getInnerIterator()->valid())
		{
			$this->getInnerIterator()->rewind();
		}
	}
}

?>