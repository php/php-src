--TEST--
SPL: LimitIterator and backward seeking
--FILE--
<?php

class NumericArrayIterator implements Iterator
{
	protected $a;
	protected $i = 0;

	public function __construct($a)
	{
		echo __METHOD__ . "\n";
		$this->a = $a;
	}

	public function valid()
	{
		echo __METHOD__ . "\n";
		return $this->i < count($this->a);
	}

	public function rewind()
	{
		echo __METHOD__ . "\n";
		$this->i = 0;
	}

	public function key()
	{
		echo __METHOD__ . "\n";
		return $this->i;
	}

	public function current()
	{
		echo __METHOD__ . "\n";
		return $this->a[$this->i];
	}

	public function next()
	{
		echo __METHOD__ . "\n";
		$this->i++;
	}
}

$it = new LimitIterator(new NumericArrayIterator(array(12, 25, 42, 56)));

foreach($it as $k => $v)
{
	var_dump($k);
	var_dump($v);
}

echo "===SEEK===\n";

$it->seek(2);

echo "===LOOP===\n";

foreach(new NoRewindIterator($it) as $k => $v)
{
	var_dump($k);
	var_dump($v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
NumericArrayIterator::__construct
NumericArrayIterator::rewind
NumericArrayIterator::valid
NumericArrayIterator::valid
NumericArrayIterator::current
NumericArrayIterator::key
int(0)
int(12)
NumericArrayIterator::next
NumericArrayIterator::valid
NumericArrayIterator::current
NumericArrayIterator::key
int(1)
int(25)
NumericArrayIterator::next
NumericArrayIterator::valid
NumericArrayIterator::current
NumericArrayIterator::key
int(2)
int(42)
NumericArrayIterator::next
NumericArrayIterator::valid
NumericArrayIterator::current
NumericArrayIterator::key
int(3)
int(56)
NumericArrayIterator::next
NumericArrayIterator::valid
===SEEK===
NumericArrayIterator::rewind
NumericArrayIterator::valid
NumericArrayIterator::next
NumericArrayIterator::valid
NumericArrayIterator::next
NumericArrayIterator::valid
NumericArrayIterator::valid
NumericArrayIterator::current
NumericArrayIterator::key
===LOOP===
int(2)
int(42)
NumericArrayIterator::next
NumericArrayIterator::valid
NumericArrayIterator::current
NumericArrayIterator::key
int(3)
int(56)
NumericArrayIterator::next
NumericArrayIterator::valid
===DONE===
