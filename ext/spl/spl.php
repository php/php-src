<?php

/** \file spl.php
 * \ingroup SPL
 * \brief Documentation of internal classes and interfaces
 *
 * SPL - Standard PHP Library
 *
 * (c) Marcus Boerger, 2003 - 2004
 */

/** \mainpage SPL - Standard PHP Library
 *
 * SPL - Standard PHP Library
 *
 * You can download this documentation as a chm file 
 * <a href="http://php.net/~helly/php/ext/spl/spl.chm">here</a>.
 *
 * (c) Marcus Boerger, 2003 - 2004
 */

/** \defgroup SPL Internal classes
 *
 * The classes and interfaces in this group are contained in the c-code of 
 * ext/SPL.
 */

/** \defgroup Examples Example classes
 *
 * The classes and interfaces in this group are contained as PHP code in the 
 * examples subdirectory of ext/SPL. Sooner or later they will be moved to
 * c-code.
 */

/** \ingroup SPL
 * \brief Interface to override array access of objects.
 */
interface ArrayAccess
{
	/** \param $offset to modify
	 * \param $value new value
	 */
	function offsetSet($offset, $value);

	/** \param $offset to retrieve
	 * \return value at given offset
	 */
	function offsetGet($offset);

	/** \param $offset to delete
	 */
	function offsetUnset($offset);

	/** \param $offset to check
	 *\return whether the offset exists.
	 */
	function offsetExists($offset);
}

/** \ingroup SPL
 * \brief Interface to detect a class is traversable using foreach.
 *
 * Abstract base interface that cannot be implemented alone. Instead it
 * must be implemented by either IteratorAggregate or Iterator. 
 *
 * \note Internal classes that implement this interface can be used in a 
 * foreach construct and do not need to implement IteratorAggregate or
 * Iterator.
 *
 * \note This is an engine internal interface which cannot be implemented
 * in PHP scripts. Either IteratorAggregate or Iterator must be used 
 * instead.
 */
interface Traversable
{
}

/** \ingroup SPL
 * \brief Interface to create an external Iterator.
 *
 * \note This is an engine internal interface.
 */ 
interface IteratorAggregate extends Traversable
{
	/** \return an Iterator for the implementing object.
	 */
	function getIterator();
}

/** \ingroup SPL
 * \brief Basic iterator
 *
 * Interface for external iterators or objects that can be iterated 
 * themselves internally.
 *
 * \note This is an engine internal interface.
 */
interface Iterator extends Traversable
{
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
	function valid();
}

/** \ingroup SPL
 * \brief Recursive iterator
 *
 * Interface for recursive traversal to be used with 
 * RecursiveIteratorIterator.
 */
interface RecursiveIterator extends Iterator
{
	/** \return whether current element can be iterated itself.
	  */
	function hasChildren();

	/** \return an object that recursively iterates the current element.
	 * This object must implement RecursiveIterator.
	 */
	function getChildren();
}

/** \ingroup SPL
 * \brief Class for recursive traversal. 
 *
 * The objects of this class are created by instances of RecursiveIterator. 
 * Elements of those iterators may be traversable themselves. If so these 
 * sub elements are recursed into.
 */
class RecursiveIteratorIterator implements Iterator
{
	/** Construct an instance form a RecursiveIterator.
	 *
	 * \param $iterator inner root iterator
	 * \param $mode one of
	 *            - RIT_LEAVES_ONLY do not return elements that can be recursed.
	 *            - RIT_SELF_FIRST  show elements before their sub elements.
	 *            - RIT_CHILD_FIRST show elements after their sub elements.
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

/** \ingroup SPL
 * \brief An Array wrapper
 *
 * This array wrapper allows to recursively iterate over Arrays and public 
 * Object properties.
 *
 * \see ArrayIterator
 */
class ArrayObject implements IteratorAggregate, ArrayAccess
{
	/** Construct a new array iterator from anything that has a hash table.
	 * That is any Array or Object.
	 *
	 * \param $array the array to use.
	 */
	function __construct($array);

	/** \return the iterator which is an ArrayIterator object connected to
	 * this object.
	 */
	function getIterator();

	/** \param $index offset to inspect
	 * \return whetehr offset $index esists
	 */	
	function offsetExists($index);

	/** \param $index offset to return value for
	 * \return value at offset $index
	 */	
	function offsetGet($index);

	/** \param $index index to set
	 * \param $newval new value to store at offset $index
	 */	
	function offsetSet($index, $newval);

	/** \param $index offset to unset
	 */	
	function offsetUnset($index);

	/** \param $value is appended as last element
	 * \warning this method cannot be called when the ArrayObject refers to 
	 *          an object.
	 */	
	function append($value);

	/** \return a \b copy of the array
	 * \note when the ArrayObject refers to an object then this method 
	 *       returns an array of the public properties.
	 */	
	function getArrayCopy();

	/** \return the number of elements in the array or the number of public
	 * properties in the object.
	 */
	function count();
}

/** \ingroup SPL
 * \brief An Array iterator
 *
 * This iterator allows to unset and modify values and keys while iterating
 * over Arrays and Objects.
 *
 * When you want to iterate over the same array multiple times you need to 
 * instanciate ArrayObject and let it create ArrayIterator instances that 
 * refer to it either by using foreach or by calling its getIterator() 
 * method manually.
 */
class ArrayIterator implements Iterator, SeekableIterator, ArrayAccess
{
	/** Construct a new array iterator from anything that has a hash table.
	 * That is any Array or Object.
	 *
	 * \param $array the array to use.
	 */
	public function __construct($array);

	/** \param $index offset to inspect
	 * \return whetehr offset $index esists
	 */	
	function offsetExists($index);

	/** \param $index offset to return value for
	 * \return value at offset $index
	 */	
	function offsetGet($index);

	/** \param $index index to set
	 * \param $newval new value to store at offset $index
	 */	
	function offsetSet($index, $newval);

	/** \param $index offset to unset
	 */	
	function offsetUnset($index);

	/** \param $value is appended as last element
	 * \warning this method cannot be called when the ArrayIterator refers to 
	 *          an object.
	 */	
	function append($value);

	/** \return a \b copy of the array
	 * \note when the ArrayIterator refers to an object then this method 
	 *       returns an array of the public properties.
	 */	
	function getArrayCopy();

	/** \param $position offset to seek to
	 */
	function seek($position);

	/** \return the number of elements in the array or the number of public
	 * properties in the object.
	 */
	function count();
}

/** \ingroup SPL
 * \brief An iterator that filters other iterators
 * 
 * Iterator that wrapps around another iterator and only returns selected
 * elements of the inner iterator. The only thing that needs to be done to
 * make this work is implementing method accept(). Typically this invloves
 * reading the current element or key of the inner Iterator and checking
 * whether it is acceptable.
 */
abstract class FilterIterator implements Iterator
{
	/** Construct an instance form a Iterator.
	 *
	 * \param $iterator inner iterator
	 */
	function __construct(Iterator $iterator);

	/** \return whether the current element of the inner iterator should be
	 * used as a current element of this iterator or if it should be skipped.
	 */
	abstract function accept();
	
	/** \return the inner Iterator
	 */
	function getInnerIterator();
}

/** \ingroup SPL
 * \brief Seekable iterator
 *
 * This interface is used to optimize LimitIterator functionality. but it 
 * may also be used for other situations where seeking a specific offset is
 * required and easily possible.
 */
interface SeekableIterator extends Iterator
{
	/** Seek to a specific position if available or throw an exception.
	 * \param $position offset to seek to.
	 */
	function seek($position);
}

/** \ingroup SPL
 * \brief Limiting iterator
 *
 * A class that starts iteration at a certain offset and only iterates over
 * a specified amount of elements.
 *
 * This class uses SeekableIterator::seek() if available and rewind() plus
 * a skip loop otehrwise.
 */
class LimitIterator implements Iterator
{
	/** Construct an instance form a Iterator.
	 *
	 * \param $iterator inner iterator
	 * \param $offset   starting position (zero based)
	 * \param $count    amount of elements returned, if available)
	 */
	function __construct(Iterator $iterator, $offset = 0, $count = -1);

	/** \return whether the current element of the inner iterator should be
	 * used as a current element of this iterator or if it should be skipped.
	 */
	abstract function accept();
	
	/** \return the inner Iterator
	 */
	function getInnerIterator();
	
	/** Seek to a specific position if available or throw an exception.
	 * If the inner iterator is an instance of SeekableIterator its seek()
	 * method will be used. Otherwise the iterator will be rewound if
	 * necessary and then manually advanced element by element.
	 *
	 * \param $position index to seek to.
	 */
	function seek($position);
	
	/** return the current position (zero based)
	 */
	function getPosition();
}

/** \ingroup SPL
 * \brief Iterator that shows only parents
 *
 * A recursive iterator that only returns elements that themselves can be 
 * trversed.
 */
class ParentIterator extends FilterIterator implements RecursiveIterator
{
	/** Construct an instance form a RecursiveIterator.
	 *
	 * \param $iterator inner iterator
	 */
	function __construct(RecursiveIterator $iterator);
}

/** \ingroup SPL
 * \brief Caching iterator
 *
 * This Iterator allways reads one ahead. That allows it to know whether
 * more elements are available.
 */
class CachingIterator implements Iterator
{
	/** Construct an instance form a RecursiveIterator.
	 *
	 * \param $iterator  inner iterator
	 * \param $getStrVal whether to fetch the value returned by __toString()
	 *                   or the (string) conversion. This is optional since
	 *                   it is not always used nad takes an additional fcall.
	 */
	function __construct(Iterator $iterator, $getStrVal = false);

	/** \return whether the inner iterator is valid. That is this iterator
	 * is valid and has one more element.
	 */
	function valid();

	/** \return The last value from the inner iterators __toString() or
	 * (string) conversion. The value is only fetched when the __constructor
	 * was called with $getStrVal = true.
	 */
	function __tostring();
	
	/** \return the inner Iterator
	 */
	function getInnerIterator();
}

/** \ingroup SPL
 * \brief The recursive version of the CachingIterator.
 */
class CachingRecursiveIterator extends CachingIterator implements RecursiveIterator
{
	/** Construct an instance form a RecursiveIterator.
	 *
	 * \param $iterator inner iterator
	 * \param $getStrVal whether to fetch the value returned by __toString()
	 *                   or the (string) conversion. This is optional since
	 *                   it is not always used nad takes an additional fcall.
	 */
	function __construct(RecursiveIterator $iterator, $getStrVal);
}

/** \ingroup SPL
 * \brief Directory iterator
 */
class DirectoryIterator implements Iterator
{
	/** Construct a directory iterator from a path-string.
	 *
	 * \param $path directory to iterate.
	 */
	function __construct($path);

	/** \return The opened path.
	 */
	function getPath();	

	/** \return The current file name.
	 */
	function getFilename();	

	/** \return The current entries path and file name.
	 */
	function getPathname();	

	/** \return The current entry's permissions.
	 */
	function getPerms();

	/** \return The current entry's inode.
	 */
	function getInode();

	/** \return The current entry's size in bytes .
	 */
	function getSize();

	/** \return The current entry's owner name.
	 */
	function getOwner();

	/** \return The current entry's group name.
	 */
	function getGroup();

	/** \return The current entry's last access time.
	 */
	function getATime();

	/** \return The current entry's last modification time.
	 */
	function getMTime();

	/** \return The current entry's last change time.
	 */
	function getCTime();

	/** \return The current entry's size in bytes .
	 */
	function getType();

	/** \return Whether the current entry is writeable.
	 */
	function isWritable();

	/** \return Whether the current entry is readable.
	 */
	function isReadable();

	/** \return Whether the current entry is executable.
	 */
	function isExecutable();

	/** \return Whether the current entry is .
	 */
	function isFile();

	/** \return Whether the current entry is a directory.
	 */
	function isDir();	

	/** \return Whether the current entry is either '.' or '..'.
	 */
	function isDot();	

	/** \return whether the current entry is a link.
	 */
	function isLink();

	/** \return getFilename()
	 */
	function __toString();
}

/** \ingroup SPL
 * \brief recursive directory iterator
 */
class RecursiveDirectoryIterator extends DirectoryIterator implements RecursiveIterator
{
	/** \return whether the current is a directory (not '.' or '..').
	 */
	function hasChildren();	

	/** \return a RecursiveDirectoryIterator for the current entry.
	 */
	function getChildren();	
}

/** \ingroup SPL
 * \brief recursive SimpleXML_Element iterator
 *
 * The SimpleXMLIterator implements the RecursiveIterator interface. This 
 * allows iteration over all elements using foreach or an appropriate while
 * construct, just like SimpleXMLElement does. When using the foreach construct,
 * you will also iterate over the subelements. For every element which
 * has subelements, hasChildren() returns true.  This will trigger a call to
 * getChildren() which returns the iterator for that sub element.
 */
class SimpleXMLIterator extends SimpleXMLElement implements RecursiveIterator
{
	/** \return whether the current node has sub nodes.
	 */
	function hasChildren();	

	/** \return a SimpleXMLIterator for the current node.
	 */
	function getChildren();	
}

?>
