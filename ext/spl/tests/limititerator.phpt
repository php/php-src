--TEST--
SPL: SeekableIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class NumericArrayIterator implements Iterator
{
	protected $a;
	protected $i;

	public function __construct($a)
	{
		echo __METHOD__ . "\n";
		$this->a = $a;
	}

	public function rewind()
	{
		echo __METHOD__ . "\n";
		$this->i = 0;
	}

	public function hasMore()
	{
		echo __METHOD__ . "\n";
		return $this->i < count($this->a);
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

class SeekableNumericArrayIterator extends NumericArrayIterator implements SeekableIterator
{
	public function seek($index)
	{
		if ($index < count($this->a)) {
			$this->i = $index;
		}
		echo __METHOD__ . '(' . $index . ")\n";
	}
}

$a = array(1, 2, 3, 4, 5);
foreach (new LimitIterator(new NumericArrayIterator($a)) as $v)
{
	print "$v\n";
}

echo "===SEEKABLE===\n";
$a = array(1, 2, 3, 4, 5);
foreach(new LimitIterator(new SeekableNumericArrayIterator($a)) as $v)
{
	print "$v\n";
}

echo "===SEEKING===\n";
$a = array(1, 2, 3, 4, 5);
$l = new LimitIterator(new SeekableNumericArrayIterator($a));
for($i = 0; $i < 5; $i++)
{
	$l->seek($i);
	print $l->current() . "\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
NumericArrayIterator::__construct
NumericArrayIterator::rewind
NumericArrayIterator::hasMore
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
1
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
2
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
3
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
4
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
5
NumericArrayIterator::next
NumericArrayIterator::hasMore
===SEEKABLE===
NumericArrayIterator::__construct
NumericArrayIterator::rewind
SeekableNumericArrayIterator::seek(0)
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
1
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
2
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
3
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
4
NumericArrayIterator::next
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
5
NumericArrayIterator::next
NumericArrayIterator::hasMore
===SEEKING===
NumericArrayIterator::__construct
SeekableNumericArrayIterator::seek(0)
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
1
SeekableNumericArrayIterator::seek(1)
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
2
SeekableNumericArrayIterator::seek(2)
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
3
SeekableNumericArrayIterator::seek(3)
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
4
SeekableNumericArrayIterator::seek(4)
NumericArrayIterator::hasMore
NumericArrayIterator::current
NumericArrayIterator::key
5
===DONE===
