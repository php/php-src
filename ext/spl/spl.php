<?php

/** Standard PHP Library
 *
 * (c) Marcus Boerger, 2003
 */

/** Abstract base interface that cannot be implemented alone. Instead it
 * must be implemented by either IteratorAggregate or Iterator. 
 *
 * \note Internal classes that implement this interface can be used in a 
 * foreach construct and do not need to implement IteratorAggregate or
 * Iterator.
 */
interface Traversable {
}

/** Interface to create an external Iterator.
 */ 
interface IteratorAggregate implements Traversable {
	/** Return an Iterator for the implementing object.
	 */
	function getIterator();
}

/** Interface for external iterators or objects that can be iterated 
 * themselves internally.
 */
interface Iterator implements Traversable {
	/** Rewind the Iterator to the first element.
	 */
	function rewind();

	/** Return the current element.
	 */
	function current();

	/** Return the key of the current element.
	 */
	function key();

	/** Move forward to next element.
	 */
	function next();

	/** Check if there is a current element after calls to rewind() or next().
	 */
	function hasMore();
}

/** Interface for recursive traversal to be used with 
 * RecursiveIteratorIterator.
 */
interface RecursiveIterator implements Iterator {
	/** \return whether current element can be iterated itself.
	  */
	function hasChildren();

	/** \return an object that recursively iterates the current element.
	 * This object must implement RecursiveIterator.
	 */
	function getChildren();
}

/** Class for recursive traversal. The objects of this class are created by 
 * instances of RecursiveIterator. Elements of those iterators may be 
 * traversable themselves. If so these sub elements are recursed into.
 */
class RecursiveIteratorIterator implements Iterator {
	/** Construct an instance form a RecursiveIterator.
	 *
	 * \param $iterator inner root iterator
	 * \param $mode one of
	 *            - RIT_LEAVES_ONLY do not return elements that can be recursed.
	 *            - RIT_SELF_FIRST  show elements before their sub elements.
	 *            - RIT_CHILD_FIRST show elements after their sub elements-
	 *
	 * \note If you want to see only those elements which have sub elements then
	 *       use a ParentIterator.
	 */
	function __construct(RecursiveIterator $iterator, $mode);

	/** \return the level of recursion (>=0).
	 */
	function getDepth();

	/** \param $level the level of the sub iterator to return.
	 * \return the current inner sub iterator or the iterator at the 
	 * specified $level.
	 */
	function getSubIterator([$level]);
}

/** \brief An Array wrapper
 *
 * This array wrapper allows to recursively iterate over Arrays and Objects.
 *
 * \see ArrayIterator
 */
class ArrayObject implements IteratorAggregate {

	/** Construct a new array iterator from anything that has a hash table.
	 * That is any Array or Object.
	 *
	 * \param $array the array to use.
	 */
	function __construct($array);

	/** Get the iterator which is a ArrayIterator object connected to this 
	 * object.
	 */
	function getIterator();
}

/** \brief An Array iterator
 *
 * This iterator allows to unset and modify values and keys while iterating
 * over Arrays and Objects.
 *
 * To use this class you must instanciate ArrayObject.
 */
class ArrayIterator implements Iterator {

	/** Construct a new array iterator from anything that has a hash table.
	 * That is any Array or Object.
	 *
	 * \param $array the array to use.
	 */
	private function __construct($array);

	/** \copydoc Iterator::rewind
	 */
	function rewind();

	/** \copydoc Iterator::current
	 */
	function current();

	/** \copydoc Iterator::key
	 */
	function key();

	/** \copydoc Iterator::next
	 */
	function next();

	/** \copydoc Iterator::hasMore
	 */
	function hasMore();
}

abstract class FilterIterator implements Iterator {
	/** Construct an instance form a Iterator.
	 *
	 * \param $iterator inner iterator
	 */
	function __construct(Iterator $iterator);

	/** \return whether the current element of the inner iterator should be
	 * used as a current element of this iterator or if it should be skipped.
	 */
	abstract function accept();

	/** \copydoc Iterator::rewind
	 */
	function rewind();

	/** \copydoc Iterator::current
	 */
	function current();

	/** \copydoc Iterator::key
	 */
	function key();

	/** \copydoc Iterator::next
	 */
	function next();

	/** \copydoc Iterator::hasMore
	 */
	function hasMore();
}

class ParentIterator extends FilterIterator implements RecursiveIterator {
	/** Construct an instance form a RecursiveIterator.
	 *
	 * \param $iterator inner iterator
	 */
	function __construct(RecursiveIterator $iterator);

	/** \copydoc RecursiveIterator::hasChildren
	 */
	function hasChildren();

	/** \copydoc RecursiveIterator::getChildren
	 */
	function getChildren();

	/** \copydoc Iterator::rewind
	 */
	function rewind();

	/** \copydoc Iterator::current
	 */
	function current();

	/** \copydoc Iterator::key
	 */
	function key();

	/** \copydoc Iterator::next
	 */
	function next();

	/** \copydoc Iterator::hasMore
	 */
	function hasMore();
}

/** \brief Directory iterator
 */
class DirectoryIterator implements Iterator {

	/** Construct a directory iterator from a path-string.
	 *
	 * \param $path directory to iterate.
	 */
	function __construct($path);

	/** \copydoc Iterator::rewind
	 */
	function rewind();

	/** \copydoc Iterator::current
	 */
	function current();

	/** \copydoc Iterator::next
	 */
	function next();

	/** \copydoc Iterator::hasMore
	 */
	function hasMore();
	
	/** \return The opened path.
	 */
	function getPath();	

	/** \return The current file name.
	 */
	function getFilename();	

	/** \return The current entries path and file name.
	 */
	function getPathname();	

	/** \return Whether the current entry is a directory.
	 */
	function isDir();	

	/** \return Whether the current entry is either '.' or '..'.
	 */
	function isDot();	
}

/** \brief Directory iterator
 */
class RecursiveDirectoryIterator extends DirectoryIterator implements RecursiveIterator {

	/** \return whether the current is a directory (not '.' or '..').
	 */
	function hasChildren();	

	/** \return a RecursiveDirectoryIterator for the current entry.
	 */
	function getChildren();	
	
}

?>