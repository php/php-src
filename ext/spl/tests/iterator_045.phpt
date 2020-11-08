--TEST--
SPL: CachingIterator and offsetSet/Unset, getCache using flag FULL_CACHE
--FILE--
<?php

class MyFoo
{
    function __toString()
    {
        return 'foo';
    }
}

class MyCachingIterator extends CachingIterator
{
    function __construct(Iterator $it, $flags = 0)
    {
        parent::__construct($it, $flags);
    }

    function testSet($ar)
    {
        echo __METHOD__ . "()\n";
        foreach($ar as $k => $v)
        {
            echo "set($k,$v)\n";
            $this->offsetSet($k, $v);
        }
    }

    function testUnset($ar)
    {
        echo __METHOD__ . "()\n";
        foreach($ar as $k => $v)
        {
            echo "unset($v)\n";
            $this->offsetUnset($v);
        }
    }

    function fill()
    {
        echo __METHOD__ . "()\n";
        foreach($this as $v) ;
    }

    function show()
    {
        echo __METHOD__ . "()\n";
        var_dump($this->getCache());
    }
}

$it = new MyCachingIterator(new ArrayIterator(array(0, 'foo'=>1, 2, 'bar'=>3, 4)));

try
{
    var_dump($it->offsetSet(0, 0));
}
catch(Exception $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

try
{
    var_dump($it->offsetUnset(0));
}
catch(Exception $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

$it = new MyCachingIterator(new ArrayIterator(array(0, 1, 2, 3)), CachingIterator::FULL_CACHE);

$checks = array(0 => 25, 1 => 42, 3 => 'FooBar');
$unsets = array(0, 2);

$it->testSet($checks);
$it->show();
$it->testUnset($unsets);
$it->show();
$it->fill();
$it->show();
$it->testSet($checks);
$it->show();
$it->testUnset($unsets);
$it->show();

?>
--EXPECT--
Exception: MyCachingIterator does not use a full cache (see CachingIterator::__construct)
Exception: MyCachingIterator does not use a full cache (see CachingIterator::__construct)
MyCachingIterator::testSet()
set(0,25)
set(1,42)
set(3,FooBar)
MyCachingIterator::show()
array(3) {
  [0]=>
  int(25)
  [1]=>
  int(42)
  [3]=>
  string(6) "FooBar"
}
MyCachingIterator::testUnset()
unset(0)
unset(2)
MyCachingIterator::show()
array(2) {
  [1]=>
  int(42)
  [3]=>
  string(6) "FooBar"
}
MyCachingIterator::fill()
MyCachingIterator::show()
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
MyCachingIterator::testSet()
set(0,25)
set(1,42)
set(3,FooBar)
MyCachingIterator::show()
array(4) {
  [0]=>
  int(25)
  [1]=>
  int(42)
  [2]=>
  int(2)
  [3]=>
  string(6) "FooBar"
}
MyCachingIterator::testUnset()
unset(0)
unset(2)
MyCachingIterator::show()
array(2) {
  [1]=>
  int(42)
  [3]=>
  string(6) "FooBar"
}
