<?php

/** \ingroup SPL
 * \brief Interface to access inner iterator of iterator wrappers
 */
interface OuterIterator extends Iterator
{
	/** \return inner iterator
	 */
    abstract function getInnerIterator();
}

?>