--TEST--
SPL: Iterator aggregating inner iterator's methods
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

    public function greaterThan($comp)
    {
        echo get_class($this) . '::' . __FUNCTION__ . '(' . $comp . ")\n";
        return $this->current() > $comp;
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
$it = new LimitIterator(new NumericArrayIterator($a), 1, 3);
foreach ($it as $v)
{
    print $v . ' is ' . ($it->greaterThan(2) ? 'greater than 2' : 'less than or equal 2') . "\n";
}

echo "===SEEKABLE===\n";
$a = array(1, 2, 3, 4, 5);
$it = new LimitIterator(new SeekableNumericArrayIterator($a), 1, 3);
foreach($it as $v)
{
    print $v . ' is ' . ($it->greaterThan(2) ? 'greater than 2' : 'less than or equal 2') . "\n";
}

echo "===STACKED===\n";
echo "Shows '2 is greater than 2' because the test is actually done with the current value which is 3.\n";
$a = array(1, 2, 3, 4, 5);
$it = new CachingIterator(new LimitIterator(new SeekableNumericArrayIterator($a), 1, 3));
foreach($it as $v)
{
    print $v . ' is ' . ($it->greaterThan(2) ? 'greater than 2' : 'less than or equal 2') . "\n";
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
NumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
2 is less than or equal 2
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
NumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
3 is greater than 2
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
NumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
4 is greater than 2
NumericArrayIterator::next
===SEEKABLE===
NumericArrayIterator::__construct
NumericArrayIterator::rewind
SeekableNumericArrayIterator::seek(1)
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
SeekableNumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
2 is less than or equal 2
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
SeekableNumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
3 is greater than 2
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
SeekableNumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
4 is greater than 2
NumericArrayIterator::next
===STACKED===
Shows '2 is greater than 2' because the test is actually done with the current value which is 3.
NumericArrayIterator::__construct
NumericArrayIterator::rewind
SeekableNumericArrayIterator::seek(1)
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
SeekableNumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
2 is greater than 2
NumericArrayIterator::next
NumericArrayIterator::valid(true)
NumericArrayIterator::current
NumericArrayIterator::key
SeekableNumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
3 is greater than 2
NumericArrayIterator::next
SeekableNumericArrayIterator::greaterThan(2)
NumericArrayIterator::current
4 is greater than 2
