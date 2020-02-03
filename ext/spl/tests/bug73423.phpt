--TEST--
Bug #73423 (Reproducible crash with GDB backtrace)
--FILE--
<?php

class foo implements \RecursiveIterator
{
    public $foo = [];

    public function current()
    {
        return current($this->foo);
    }

    public function key()
    {
        return key($this->foo);
    }

    public function next()
    {
        next($this->foo);
    }

    public function rewind()
    {
        reset($this->foo);
    }

    public function valid()
    {
        return current($this->foo) !== false;
    }

    public function getChildren()
    {
        return current($this->foo);
    }

    public function hasChildren()
    {
        return (bool) count($this->foo);
    }
}


class fooIterator extends \RecursiveFilterIterator
{
    public function __destruct()
    {
        eval("class A extends NotExists {}");

        /* CRASH */
    }

    public function accept()
    {
        return true;
    }
}


$foo = new foo();

$foo->foo[] = new foo();

foreach (new \RecursiveIteratorIterator(new fooIterator($foo)) as $bar) {
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Class 'NotExists' not found in %s:%d
Stack trace:
#0 %s(%d): eval()
#1 %s(%d): fooIterator->__destruct()
#2 {main}

Next Error: Class 'NotExists' not found in %s:%d
Stack trace:
#0 %s(%d): eval()
#1 %s(%d): fooIterator->__destruct()
#2 {main}
  thrown in %s on line %d
