<?php

/** @file outeriterator.inc
 * @ingroup SPL
 * @brief class OuterIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Interface to access the current inner iteraor of iterator wrappers
 * @author  Marcus Boerger
 * @version 1.0
 * @since PHP 5.1
 */
interface OuterIterator extends Iterator
{
	/** @return inner iterator
	 */
    function getInnerIterator();
}

?>