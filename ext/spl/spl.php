<?php

/** @file spl.php
 * @ingroup SPL
 * @brief Documentation of internal classes and interfaces
 *
 * SPL - Standard PHP Library
 *
 * (c) Marcus Boerger, 2003 - 2004
 */

/** @mainpage SPL - Standard PHP Library
 *
 * SPL - Standard PHP Library
 *
 * SPL is a collection of interfaces and classes that are meant to solve 
 * standard problems and implements some efficient data access interfaces 
 * and classes. You'll find the classes documented using php code in the 
 * file spl.php or in corresponding .inc files in subdirectories examples 
 * and internal. Based on the internal implementations or the files in the 
 * examples subdirectory there are also some .php files to experiment with.
 * 
 * The .inc files are not included automatically because they are sooner or 
 * later integrated into the extension. That means that you either need to 
 * put the code of examples/autoload.inc into your autoprepend file or that 
 * you have to point your ini setting auto_prepend_file to that file.
 * 
 * Below is a list of interfaces/classes already availabel natively through
 * the SPL extension grouped by category.
 * 
 * 1) Iterators
 * 
 * SPL offers some advanced iterator algorithms:
 * 
 * - interface RecursiveIterator implements Iterator
 * - interface OuterIterator extends Iterator
 * - class RecursiveIteratorIterator implements OuterIterator
 * - abstract class FilterIterator implements OuterIterator
 * - class ParentIterator extends FilterIterator implements RecursiveIterator
 * - interface SeekableIterator implements Iterator
 * - class LimitIterator implements OuterIterator
 * - class CachingIterator implements OuterIterator
 * - class CachingRecursiveIterator extends CachingIterator implements RecursiveIterator
 * - class IteratorIterator implements OuterIterator
 * - class NoRewindIterator implements OuterIterator
 * - class EmptyIterator implements Iterator
 * - class InfiniteIterator extends IteratorIterator
 * - class AppendIterator implements OuterIterator
 * 
 * 2) Directories
 * 
 * SPL offers two advanced directory classes:
 * 
 * - class DirectoryIterator implements Iterator
 * - class RecursiveDirectoryIterator extends DirectoryIterator implements RecursiveIterator
 * 
 * 3) XML
 * 
 * SPL offers an advanced XML handling class:
 * 
 * - class SimpleXMLIterator extends simplexml_element extends recursiveiterator
 * 
 * 4) Array Overloading
 * 
 * SPL offers advanced Array overloading:
 * 
 * - class ArrayObject implements IteratorAggregate
 * - class ArrayIterator implements Iterator
 *
 * 5) Counting
 *
 * - interface Counable allows to hook into the standard array function count().
 *
 * 6) Exceptions
 *
 * SPL provides a set of standard exception classes each meant to indicate a 
 * certain problem type.
 *
 * - class LogicException           extends Exception
 * - class BadFunctionCallException extends LogicException
 * - class BadMethodCallException   extends BadFunctionCallException
 * - class DomainException          extends LogicException
 * - class InvalidArgumentException extends LogicException
 * - class LengthException          extends LogicException
 * - class OutOfRangeException      extends LogicException
 * - class RuntimeException         extends Exception
 * - class OutOfBoundsException     extends RuntimeException
 * - class OverflowException        extends RuntimeException
 * - class RangeException           extends RuntimeException
 * - class UnderflowException       extends RuntimeException
 * 
 * As the above suggest an ArrayObject creates an ArrayIterator when it comes to
 * iteration (e.g. ArrayObject instance used inside foreach). 
 * 
 * A nice article about SPL can be found 
 * <a href="http://www.sitepoint.com/article/php5-standard-library/1">here</a>.
 *
 * You can download this documentation as a chm file 
 * <a href="http://php.net/~helly/php/ext/spl/spl.chm">here</a>.
 *
 * (c) Marcus Boerger, 2003 - 2004
 */

/** @defgroup ZendEngine Zend engine classes
 *
 * The classes and interfaces in this group are contained in the c-code of 
 * PHP's Zend engine.
 */

/** @defgroup SPL Internal classes
 *
 * The classes and interfaces in this group are contained in the c-code of 
 * ext/SPL.
 */

/** @defgroup Examples Example classes
 *
 * The classes and interfaces in this group are contained as PHP code in the 
 * examples subdirectory of ext/SPL. Sooner or later they will be moved to
 * c-code.
 */

/** @ingroup ZendEngine
 * @brief Basic Exception class.
 */
class Exception
{
	/** The exception message */
    protected $message;
    
    /** The string represenations as generated during construction */
    private $string;
    
    /** The code passed to the constructor */
    protected $code;
    
    /** The file name where the exception was instantiated */
    protected $file;
    
    /** The line number where the exception was instantiated */
    protected $line;
    
    /** The stack trace */
    private $trace;

	/** Prevent clone
	 */
    final private function __clone() {}

	/** Construct an exception
	 *
	 * @param $message Some text describing the exception
	 * @param $code    Some code describing the exception
	 */
	function __construct($message = NULL, $code = 0) {
		if (func_num_args()) {
			$this->message = $message;
		}
		$this->code = $code;
		$this->file = __FILE__; // of throw clause
		$this->line = __LINE__; // of throw clause
		$this->trace = debug_backtrace();
		$this->string = StringFormat($this);
	} 

	/** @return the message passed to the constructor
	 */
    final public function getMessage()
    {
    	return $this->message;
    }

	/** @return the code passed to the constructor
	 */
    final public function getCode()
    {
    	return $this->code;
    }

	/** @return the name of the file where the exception was thrown
	 */
    final public function getFile()
    {
    	return $this->file;
    }

	/** @return the line number where the exception was thrown
	 */
    final public function getLine()
    {
    	return $this->line;
    }

	/** @return the stack trace as array
	 */
    final public function getTrace()
    {
    	return $this->trace;
    }

	/** @return the stack trace as string
	 */
    final public function getTraceAsString()
    {
    }

	/** @return string represenation of exception
	 */
    public function __toString()
    {
    	return $this->string;
    }
}

/** @ingroup SPL
 * @brief Exception that represents error in the program logic.
 *
 * This kind of exceptions should directly leed to a fix in your code.
 */
class LogicException extends Exception
{
}

/** @ingroup SPL
 * @brief Exception thrown when a function call was illegal.
 */
class BadFunctionCallException extends LogicException
{
}

/** @ingroup SPL
 * @brief Exception thrown when a method call was illegal.
 */
class BadMethodCallException extends BadFunctionCallException
{
}

/** @ingroup SPL
 * @brief Exception that denotes a value not in the valid domain was used.
 *
 * This kind of exception should be used to inform about domain erors in 
 * mathematical sense.
 */
class DomainException extends LogicException
{
}

/** @ingroup SPL
 * @brief Exception that denotes invalid arguments were passed.
 */
class InvalidArgumentException extends LogicException
{
}

/** @ingroup SPL
 * @brief Exception thrown when a parameter exceeds the allowed length.
 *
 * This can be used for strings length, array size, file size, number of 
 * elements read from an Iterator and so on.
 */
class LengthException extends LogicException
{
}

/** @ingroup SPL
 * @brief Exception thrown when an illegal index was requested.
 *
 * This represents errors that should be detected at compile time.
 *
 * @see OutOfBoundsException
 */
class OutOfRangeException extends LogicException
{
}

/** @ingroup SPL
 * @brief Exception thrown for errors that are only detectable at runtime. 
 */
class RuntimeException extends Exception
{
}

/** @ingroup SPL
 * @brief Exception thrown when an illegal index was requested.
 *
 * This represents errors that cannot be detected at compile time.
 *
 * @see OutOfRangeException
 */
class OutOfBoundsException extends RuntimeException
{
}

/** @ingroup SPL
 * @brief Exception thrown to indicate arithmetic/buffer overflow.
 */
class OverflowException extends RuntimeException
{
}

/** @ingroup SPL
 * @brief Exception thrown to indicate range errors during program execution.
 *
 * Normally this means there was an arithmetic error other than under/overflow.
 */
class RangeException extends RuntimeException
{
}

/** @ingroup SPL
 * @brief Exception Exception thrown to indicate arithmetic/buffer underflow.
 */
class UnderflowException extends RuntimeException
{
}

/** @ingroup ZendEngine
 * @brief Interface to override array access of objects.
 */
interface ArrayAccess
{
	/** @param $offset to modify
	 * @param $value new value
	 */
	function offsetSet($offset, $value);

	/** @param $offset to retrieve
	 * @return value at given offset
	 */
	function offsetGet($offset);

	/** @param $offset to delete
	 */
	function offsetUnset($offset);

	/** @param $offset to check
	 * @return whether the offset exists.
	 */
	function offsetExists($offset);
}

/** @ingroup ZendEngine
 * @brief Interface to detect a class is traversable using foreach.
 *
 * Abstract base interface that cannot be implemented alone. Instead it
 * must be implemented by either IteratorAggregate or Iterator. 
 *
 * @note Internal classes that implement this interface can be used in a 
 * foreach construct and do not need to implement IteratorAggregate or
 * Iterator.
 *
 * @note This is an engine internal interface which cannot be implemented
 * in PHP scripts. Either IteratorAggregate or Iterator must be used 
 * instead.
 */
interface Traversable
{
}

/** @ingroup ZendEngine
 * @brief Interface to create an external Iterator.
 *
 * @note This is an engine internal interface.
 */ 
interface IteratorAggregate extends Traversable
{
	/** @return an Iterator for the implementing object.
	 */
	function getIterator();
}

/** @ingroup ZendEngine
 * @brief Basic iterator
 *
 * Interface for external iterators or objects that can be iterated 
 * themselves internally.
 *
 * @note This is an engine internal interface.
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

/** @ingroup SPL
 * @brief This Interface allows to hook into the global count() function.
 */
interface Countable
{
	/** @return the number the global function count() should show
	 */
	function count();
}

/** @ingroup SPL
 * @brief An Array wrapper
 *
 * This array wrapper allows to recursively iterate over Arrays and public 
 * Object properties.
 *
 * @see ArrayIterator
 */
class ArrayObject implements IteratorAggregate, ArrayAccess, Countable
{
	/** Construct a new array iterator from anything that has a hash table.
	 * That is any Array or Object.
	 *
	 * @param $array the array to use.
	 */
	function __construct($array);

	/** @return the iterator which is an ArrayIterator object connected to
	 * this object.
	 */
	function getIterator();

	/** @param $index offset to inspect
	 * @return whetehr offset $index esists
	 */	
	function offsetExists($index);

	/** @param $index offset to return value for
	 * @return value at offset $index
	 */	
	function offsetGet($index);

	/** @param $index index to set
	 * @param $newval new value to store at offset $index
	 */	
	function offsetSet($index, $newval);

	/** @param $index offset to unset
	 */	
	function offsetUnset($index);

	/** @param $value is appended as last element
	 * @warning this method cannot be called when the ArrayObject refers to 
	 *          an object.
	 */	
	function append($value);

	/** @return a \b copy of the array
	 * @note when the ArrayObject refers to an object then this method 
	 *       returns an array of the public properties.
	 */	
	function getArrayCopy();

	/** @return the number of elements in the array or the number of public
	 * properties in the object.
	 */
	function count();
}

/** @ingroup SPL
 * @brief An Array iterator
 *
 * This iterator allows to unset and modify values and keys while iterating
 * over Arrays and Objects.
 *
 * When you want to iterate over the same array multiple times you need to 
 * instanciate ArrayObject and let it create ArrayIterator instances that 
 * refer to it either by using foreach or by calling its getIterator() 
 * method manually.
 */
class ArrayIterator implements SeekableIterator, ArrayAccess, Countable
{
	/** Construct a new array iterator from anything that has a hash table.
	 * That is any Array or Object.
	 *
	 * @param $array the array to use.
	 */
	public function __construct($array);

	/** @param $index offset to inspect
	 * @return whetehr offset $index esists
	 */	
	function offsetExists($index);

	/** @param $index offset to return value for
	 * @return value at offset $index
	 */	
	function offsetGet($index);

	/** @param $index index to set
	 * @param $newval new value to store at offset $index
	 */	
	function offsetSet($index, $newval);

	/** @param $index offset to unset
	 */	
	function offsetUnset($index);

	/** @param $value is appended as last element
	 * @warning this method cannot be called when the ArrayIterator refers to 
	 *          an object.
	 */	
	function append($value);

	/** @return a \b copy of the array
	 * @note when the ArrayIterator refers to an object then this method 
	 *       returns an array of the public properties.
	 */	
	function getArrayCopy();

	/** @param $position offset to seek to
	 */
	function seek($position);

	/** @return the number of elements in the array or the number of public
	 * properties in the object.
	 */
	function count();
}

/** @ingroup SPL
 * @brief Directory iterator
 */
class DirectoryIterator implements Iterator
{
	/** Construct a directory iterator from a path-string.
	 *
	 * @param $path directory to iterate.
	 */
	function __construct($path);

	/** @return The opened path.
	 */
	function getPath();	

	/** @return The current file name.
	 */
	function getFilename();	

	/** @return The current entries path and file name.
	 */
	function getPathname();	

	/** @return The current entry's permissions.
	 */
	function getPerms();

	/** @return The current entry's inode.
	 */
	function getInode();

	/** @return The current entry's size in bytes .
	 */
	function getSize();

	/** @return The current entry's owner name.
	 */
	function getOwner();

	/** @return The current entry's group name.
	 */
	function getGroup();

	/** @return The current entry's last access time.
	 */
	function getATime();

	/** @return The current entry's last modification time.
	 */
	function getMTime();

	/** @return The current entry's last change time.
	 */
	function getCTime();

	/** @return The current entry's size in bytes .
	 */
	function getType();

	/** @return Whether the current entry is writeable.
	 */
	function isWritable();

	/** @return Whether the current entry is readable.
	 */
	function isReadable();

	/** @return Whether the current entry is executable.
	 */
	function isExecutable();

	/** @return Whether the current entry is .
	 */
	function isFile();

	/** @return Whether the current entry is a directory.
	 */
	function isDir();	

	/** @return Whether the current entry is either '.' or '..'.
	 */
	function isDot();	

	/** @return whether the current entry is a link.
	 */
	function isLink();

	/** @return getFilename()
	 */
	function __toString();
}

/** @ingroup SPL
 * @brief recursive directory iterator
 */
class RecursiveDirectoryIterator extends DirectoryIterator implements RecursiveIterator
{
	/** @return whether the current is a directory (not '.' or '..').
	 */
	function hasChildren();	

	/** @return a RecursiveDirectoryIterator for the current entry.
	 */
	function getChildren();	
}

/** @ingroup SPL
 * @brief recursive SimpleXML_Element iterator
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
	/** @return whether the current node has sub nodes.
	 */
	function hasChildren();	

	/** @return a SimpleXMLIterator for the current node.
	 */
	function getChildren();	
}

?>
