<?php

/** @file outeriterator.inc
 * @ingroup Internal
 * @brief class OuterIterator
 * @author  Marcus Boerger
 * @date    2003 - 2004
 *
 * SPL - Standard PHP Library
 */

/**
 * @brief   Interface to access the current inner iteraor of iterator wrappers
 * @author  Marcus Boerger
 * @version 1.0
 */
interface OuterIterator extends Iterator
{
	/** @return inner iterator
	 */
    function getInnerIterator();
}

?>