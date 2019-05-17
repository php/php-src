--TEST--
Bug #37457 (Crash when an exception is thrown in accept() method of FilterIterator)
--FILE--
<?php

class Collection implements Iterator
{
	protected $array, $valid = false;

	public function __construct(array $a)
	{
		echo __METHOD__ . "\n";
		$this->array = $a;
	}

	public function current()
	{
		echo __METHOD__ . "\n";
		return current($this->array);
	}

	public function key()
	{
		echo __METHOD__ . "\n";
		return key($this->array);
	}

	public function next()
	{
		echo __METHOD__ . "\n";
		$this->valid = (false !== next($this->array));
	}

	public function valid()
	{
		echo __METHOD__ . "\n";
		return $this->valid;
	}

	public function rewind()
	{
		echo __METHOD__ . "\n";
		$this->valid = (false !== reset($this->array));
	}
}

class TestFilter extends FilterIterator
{
    public function accept()
    {
		echo __METHOD__ . "\n";
    	throw new Exception("Failure in Accept");
    }
}

$test = new TestFilter(new Collection(array(0)));

try
{
	foreach ($test as $item)
	{
		echo $item;
	}
}
catch (Exception $e)
{
	var_dump($e->getMessage());
}

?>
===DONE===
--EXPECT--
Collection::__construct
Collection::rewind
Collection::valid
Collection::current
Collection::key
TestFilter::accept
string(17) "Failure in Accept"
===DONE===
