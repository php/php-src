--TEST--
Test typed properties add array element guard
--FILE--
<?php
$foo = new class
{
    public int $bar = 42;

    public function getIterator()
    {
        foreach(['1', &$this->bar] as $item)
        {
            yield $item;
        }
    }
};

foreach($foo->getIterator() as $item);
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property class@anonymous::$bar must not be referenced in %s:8
Stack trace:
#0 %s(15): class@anonymous->getIterator()
#1 {main}
  thrown in %s on line 8
