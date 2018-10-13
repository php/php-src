<?php

/** @file splstack.inc
 * @ingroup SPL
 * @brief class SplStack
 * @author  Etienne Kneuss
 * @date    2008 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief Implementation of a stack through a DoublyLinkedList. As SplStack
 *        extends SplDoublyLinkedList, shift() and unshift() are still available even
 *        though they don't make much sense for a stack.
 * @since PHP 5.3
 *
 * The SplStack class provides the main functionalities of a
 * stack implemented using a doubly linked list (DLL).
 */
class SplStack extends SplDoublyLinkedList
{
	protected $_it_mode = parent::IT_MODE_LIFO;

	/** Changes the iteration mode. There are two orthogonal sets of modes that
	 * can be set:
	 *
	 * - The behavior of the iterator (either one or the other)
	 *  - SplDoublyLnkedList::IT_MODE_DELETE (Elements are deleted by the iterator)
	 *  - SplDoublyLnkedList::IT_MODE_KEEP   (Elements are traversed by the iterator)
	 *
	 * The default mode is 0 : SplDoublyLnkedList::IT_MODE_LIFO | SplDoublyLnkedList::IT_MODE_KEEP
	 *
	 * @note The iteration's direction is not modifiable for stack instances
	 * @param $mode              New mode of iteration
	 * @throw RuntimeException   If the new mode affects the iteration's direction.
	 */
	public function setIteratorMode($mode)
	{
		if ($mode & parent::IT_MODE_LIFO !== parent::IT_MODE_LIFO) {
			throw new RuntimeException("Iterators' LIFO/FIFO modes for SplStack/SplQueue objects are frozen");
		}

		$this->_it_mode = $mode;
	}
}

?>
