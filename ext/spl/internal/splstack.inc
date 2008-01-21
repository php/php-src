<?php

/** @file splstack.inc
 * @ingroup SPL
 * @brief class SplStack
 * @author  Etienne Kneuss
 * @date    2008
 *
 * SPL - Standard PHP Library
 */


/** @ingroup SPL
 * @brief Implementation of a stack through a DoublyLinkedList. As SplStack 
 * extends SplDoublyLinkedList, shift() and unshift() are still available even
 * though they don't make much sense for a stack.
 *
 * @since PHP 5.3
 *
 * The SplStack class provides the main functionnalities of a
 * stack implemented by a doubly linked list.
 */
class SplStack extends SplDoublyLinkedList
{
	/** Changes the iteration mode. For stacks, the direction mode
	 * is frozen. Attempting to modify it will result in an RuntimeException.
	 *
	 * @throws RuntimeException
	 * @param $mode new mode of iteration
	 * @see SplDoublyLinkedList::setIteratorMode
	 */
	function setIteratorMode($mode) {/**/}
}

?>
