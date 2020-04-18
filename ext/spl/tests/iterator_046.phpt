--TEST--
SPL: CachingIterator and __toString using bypassed string keys
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

    function fill()
    {
        echo __METHOD__ . "()\n";
        foreach($this as $v) ;
    }

    function show()
    {
        echo __METHOD__ . "()\n";
        foreach($this as $v)
        {
            var_dump((string)$this);
        }
    }
}

$it = new MyCachingIterator(new ArrayIterator(array(0, 'foo'=>1, 'bar'=>2)), CachingIterator::TOSTRING_USE_KEY);

$it->fill();
$it->show();

?>
--EXPECT--
MyCachingIterator::fill()
MyCachingIterator::show()
string(1) "0"
string(3) "foo"
string(3) "bar"
