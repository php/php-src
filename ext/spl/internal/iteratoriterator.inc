<?php

/** @file iteratoriterator.inc
 * @ingroup SPL
 * @brief class IteratorIterator
 * @author  Marcus Boerger
 * @date    2003 - 2009
 *
 * SPL - Standard PHP Library
 */

/** @ingroup SPL
 * @brief Basic Iterator wrapper
 * @since PHP 5.1
 *
 * This iterator wrapper allows to convert anything that is traversable into 
 * an Iterator. It is very important to understand that most classes that do 
 * not implement Iterator have their reasone to. Most likely they do not allow
 * the full Iterator feature set. If so you need to provide techniques to
 * prevent missuse. If you do not you must expect exceptions or fatal erros.
 *
 * It is also possible to derive the class and implement IteratorAggregate by
 * downcasting the instances returned in getIterator. See the following
 * example (assuming BaseClass implements Traversable):
 \code
 class SomeClass extends BaseClass implements IteratorAggregate
 {
   function getIterator()
   {
     return new IteratorIterator($this, 'BaseClass');
   }
 }
 \endcode
 *
 * As you can see in the example this approach requires that the class to 
 * downcast to is actually a base class of the specified iterator to wrap.
 * Omitting the downcast in the above example would result in an endless loop
 * since IteratorIterator::__construct() would call SomeClass::getIterator().
 */
class IteratorIterator implements OuterIterator
{
	/** Construct an IteratorIterator from an Iterator or an IteratorAggregate.
	 *
	 * @param iterator  inner iterator
	 * @param classname optional class the iterator has to be downcasted to
	 */
	function __construct(Traversable $iterator, $classname = null)
	{
		if ($iterator instanceof IteratorAggregate)
		{
		    $iterator = $iterator->getIterator();
		}
		if ($iterator instanceof Iterator)
		{
			$this->iterator = $iterator;
		}
		else
		{
			throw new Exception("Classes that only implement Traversable can be wrapped only after converting class IteratorIterator into c code");
		}
	}

	/** \return the inner iterator as passed to the constructor
	 */
	function getInnerIterator()
	{
		return $this->iterator;
	}

	/** \return whether the iterator is valid
	 */
	function valid()
	{
		return $this->iterator->valid();
	}

	/** \return current key
	 */
	function key()
	{
		return $this->iterator->key();
	}

	/** \return current value
	 */
	function current()
	{
		return $this->iterator->current();
	}

	/** forward to next element
	 */
	function next()
	{
		return $this->iterator->next();
	}

	/** rewind to the first element
	 */
	function rewind()
	{
		return $this->iterator->rewind();
	}

	/** Aggregate the inner iterator
	 *
	 * @param func    Name of method to invoke
	 * @param params  Array of parameters to pass to method
	 */
	function __call($func, $params)
	{
		return call_user_func_array(array($this->iterator, $func), $params);
	}

	/** The inner iterator must be private because when this class will be
	 * converted to c code it won't no longer be available.
	 */
	private $iterator;
}

?>
