<?php

/**
 * @brief   An infinite Iterator
 * @author  Marcus Boerger
 * @version 1.0
 *
 * This Iterator takes another Iterator and infinitvely iterates it by
 * rewinding it when its end is reached.
 *
 * \note Even an InfiniteIterator stops if its inner Iterator is empty.
 *
 \verbatim
 $it       = new ArrayIterator(array(1,2,3));
 $infinite = new InfiniteIterator($it);
 $limit    = new LimitIterator($infinite, 0, 5);
 foreach($limit as $val=>$key)
 {
 	echo "$val=>$key\n";
 }
 \endverbatim
 */
class InfiniteIterator implements Iterator
{
	private $it;

	function __construct(Iterator $it)
	{
		$this->it = $it;
	}

	function getInnerIterator()
	{
		return $this->it;
	}

	function rewind()
	{
		$this->it->rewind();
	}

	function valid()
	{
		return $this->it->valid();
	}

	function current()
	{
		return $this->it->current();
	}

	function key()
	{
		return $this->it->key();
	}

	function next()
	{
		$this->it->next();
		if (!$this->it->valid())
		{
			$this->it->rewind();
		}
	}
}

?>