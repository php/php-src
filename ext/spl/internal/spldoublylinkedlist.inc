<?php

/** @file spldoublylinkedlist.inc
 * @ingroup SPL
 * @brief class SplDoublyLinkedList
 * @author  Etienne Kneuss
 * @date    2008
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief Doubly Linked List
 * @since PHP 5.3
 *
 * The SplDoublyLinkedList class provides the main functionnalities of a
 * doubly linked list (DLL).
 */
class SplDoublyLinkedList implements Traversable, ArrayAccess, Countable
{

	/** Iterator mode
	 * @see setIteratorMode
	 */
	const IT_MODE_LIFO     = 0x00000001;

	/** Iterator mode
	 * @see setIteratorMode
	 */
	const IT_MODE_FIFO     = 0x00000000;


	/** Iterator mode
	 * @see setIteratorMode
	 */
	const IT_MODE_KEEP     = 0x00000000;

	/** Iterator mode
	 * @see setIteratorMode
	 */
	const IT_MODE_DELETE   = 0x00000002;

	/** @return the element popped from the end of the DLL.
	 */
	function pop() {/**/}

	/** @return the element shifted from the beginning of the DLL.
	 */
	function shift() {/**/}

	/** Pushes an element to the end of the DLL.
	 * @param $data variable to add to the DLL.
	 */
	function push($data) {/**/}

	/** Adds an element to the beginning of the DLL.
	 * @param $data variable to add to the DLL.
	 */
	function unshift($data) {/**/}

	/** @return the element at the beginning of the DLL.
	 */
	function top() {/**/}

	/** @return the element at the end of the DLL.
	 */
	function bottom() {/**/}

	/** @return number elements in the DLL.
	 */
	function count() {/**/}

	/** @return whether the DLL is empty.
	 */
	function isEmpty() {/**/}

	/** Changes the iteration mode. There are two orthogonal sets of modes that 
	 * can be set:
	 * - The direction of the iteration (either one or the other)
	 *  - SplDoublyLnkedList::IT_MODE_LIFO (Stack style)
	 *  - SplDoublyLnkedList::IT_MODE_FIFO (Queue style)
	 *
	 * - The behavior of the iterator (either one or the other)
	 *  - SplDoublyLnkedList::IT_MODE_DELETE (Elements are deleted by the iterator)
	 *  - SplDoublyLnkedList::IT_MODE_KEEP   (Elements are traversed by the iterator)
	 *
	 * The default mode is 0 : SplDoublyLnkedList::IT_MODE_FIFO | SplDoublyLnkedList::IT_MODE_KEEP
	 *
	 * @param $mode new mode of iteration
	 */
	function setIteratorMode($mode) {/**/}

	/** @return the current iteration mode
	 * @see setIteratorMode
	 */
	function getIteratorMode() {/**/}

}

?>
