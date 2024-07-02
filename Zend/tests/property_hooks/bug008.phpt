--TEST--
Assign by reference to backed property is forbidden for &get-only
--FILE--
<?php

class Foo {
    private $_bar;
    public $bar {
        &get {
            echo __METHOD__, PHP_EOL;
            return $this->_bar;
        }
    }
}

$foo = new Foo;
$foo->bar = 'bar';

?>
--EXPECTF--
Fatal error: Uncaught Error: Property Foo::$bar is read-only in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
