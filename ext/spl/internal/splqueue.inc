<?php

/** @file splqueue.inc
 * @ingroup SPL
 * @brief class SplQueue
 * @author  Etienne Kneuss
 * @date    2008
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief Implementation of a Queue through a DoublyLinkedList. As SplQueue 
 * extends SplDoublyLinkedList, unshift() and pop() are still available even
 * though they don't make much sense for a queue. For convenience, two aliases
 * are available:
 * - enqueue() is an alias of push()
 * - dequeue() is an alias of shift()
 *
 * @since PHP 5.3
 *
 * The SplQueue class provides the main functionnalities of a
 * queue implemented by a doubly linked list.
 */
class SplQueue extends SplDoublyLinkedList
{
	/** Changes the iteration mode. For queues, the direction mode
	 * is frozen. Attempting to modify it will result in an RuntimeException.
	 *
	 * @throws RuntimeException
	 * @param $mode new mode of iteration
	 * @see SplDoublyLinkedList::setIteratorMode
	 */
	function setIteratorMode($mode) {/**/}

	/** @return the first element of the queue.
	 * @note dequeue is an alias of push()
	 * @see splDoublyLinkedList::push()
	 */
	function dequeue() {/**/}

	/** Pushes an element at the end of the queue.
	 * @param $data variable to add to the queue.
	 * @note enqueue is an alias of shift()
	 * @see splDoublyLinkedList::shift()
	 */
	function enqueue($data) {/**/}
}

?>
