<?php

/** @file splqueue.inc
 * @ingroup SPL
 * @brief class SplQueue
 * @author  Etienne Kneuss
 * @date    2008 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief Implementation of a Queue through a DoublyLinkedList. As SplQueue
 *        extends SplDoublyLinkedList, unshift() and pop() are still available 
 *        even though they don't make much sense for a queue. For convenience,
 *        two aliases are available:
 *         - enqueue() is an alias of push()
 *         - dequeue() is an alias of shift()
 *
 * @since PHP 5.3
 *
 * The SplQueue class provides the main functionalities of a
 * queue implemented using a doubly linked list (DLL).
 */
class SplQueue extends SplDoublyLinkedList
{
	protected $_it_mode = parent::IT_MODE_FIFO;

	/** Changes the iteration mode. There are two orthogonal sets of modes that 
	 * can be set:
	 *
	 * - The behavior of the iterator (either one or the other)
	 *  - SplDoublyLnkedList::IT_MODE_DELETE (Elements are deleted by the iterator)
	 *  - SplDoublyLnkedList::IT_MODE_KEEP   (Elements are traversed by the iterator)
	 *
	 * The default mode is 0 : SplDoublyLnkedList::IT_MODE_LIFO | SplDoublyLnkedList::IT_MODE_KEEP
	 *
	 * @note The iteration's direction is not modifiable for queue instances
	 * @param $mode              New mode of iteration
	 * @throw RuntimeException   If the new mode affects the iteration's direction.
	 */
	public function setIteratorMode($mode)
	{
		if ($mode & parent::IT_MODE_LIFO === parent::IT_MODE_LIFO) {
			throw new RuntimeException("Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen");
		}

		$this->_it_mode = $mode;
	}

	/** @return the first element of the queue.
	 * @note dequeue is an alias of push()
	 * @see splDoublyLinkedList::push()
	 */
	public function dequeue()
	{
		return parent::shift();
	}

	/** Pushes an element at the end of the queue.
	 * @param $data variable to add to the queue.
	 * @note enqueue is an alias of shift()
	 * @see splDoublyLinkedList::shift()
	 */
	public function enqueue($data)
	{
		return parent::push($data);
	}
}

?>
