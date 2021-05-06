--TEST--
SPL: SeekableIterator and string keys
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

    public function rewind(): void
    {
        echo __METHOD__ . "\n";
        $this->i = 0;
    }

    public function valid(): bool
    {
        $ret = $this->i < count($this->a);
        echo __METHOD__ . '(' . ($ret ? 'true' : 'false') . ")\n";
        return $ret;
    }

    public function key(): mixed
    {
        echo __METHOD__ . "\n";
        return $this->i;
    }

    public function current(): mixed
    {
        echo __METHOD__ . "\n";
        return $this->a[$this->i];
    }

    public function next(): void
    {
        echo __METHOD__ . "\n";
        $this->i++;
    }
}

class SeekableNumericArrayIterator extends NumericArrayIterator implements SeekableIterator
{
    public function seek($index): void
    {
        if ($index < count($this->a)) {
            $this->i = $index;
        }
        echo __METHOD__ . '(' . $index . ")\n";
    }
}

$a = array(1, 2, 3, 4, 5);
foreach (new LimitIterator(new NumericArrayIterator($a), 1, 3) as $v)
{
    print "$v\n";
}

echo "===SEEKABLE===\n";
$a = array(1, 2, 3, 4, 5);
foreach(new LimitIterator(new SeekableNumericArrayIterator($a), 1, 3) as $v)
{
    print "$v\n";
}

echo "===SEEKING===\n";
$a = array(1, 2, 3, 4, 5);
$l = new LimitIterator(new SeekableNumericArrayIterator($a));
for($i = 1; $i < 4; $i++)
{
    $l->seek($i);
    print $l->current() . "\n";
}

?>
--EXPECT--
NumericArrayIterator::__construct
NumericArrayIterator::rewind
NumericArrayIterator::valid(true)
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
2
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
3
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
4
NumericArrayIterator::next
===SEEKABLE===
NumericArrayIterator::__construct
NumericArrayIterator::rewind
SeekableNumericArrayIterator::seek(1)
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
2
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
3
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
4
NumericArrayIterator::next
===SEEKING===
NumericArrayIterator::__construct
SeekableNumericArrayIterator::seek(1)
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
2
SeekableNumericArrayIterator::seek(2)
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
3
SeekableNumericArrayIterator::seek(3)
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
4
