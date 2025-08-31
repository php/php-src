--TEST--
Assign by reference to backed property is allowed for &get-only
--FILE--
<?php

class Foo {
    public $bar = [] {
        &get {
            echo __METHOD__ . "\n";
            return $this->bar;
        }
    }
}

$foo = new Foo;
$foo->bar[] = 'bar';
var_dump($foo);

?>
--EXPECTF--
Foo::$bar::get
object(Foo)#%d (1) {
  ["bar"]=>
  array(1) {
    [0]=>
    string(3) "bar"
  }
}
