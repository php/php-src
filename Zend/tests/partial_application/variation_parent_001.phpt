--TEST--
Partial application variation parent
--FILE--
<?php
class Foo {
    public function method($a, $b, ...$c) {
        return function() {
            return $this;
        };
    }
}

$foo = new Foo();
$bar = $foo->method(10, ...);
$baz = $bar(20, ...);

var_dump($baz, $baz());
?>
--EXPECTF--
object(Closure)#%d (3) {
  ["this"]=>
  object(Foo)#%d (0) {
  }
  ["parameter"]=>
  array(1) {
    ["$c"]=>
    string(10) "<optional>"
  }
  ["args"]=>
  array(3) {
    ["a"]=>
    int(10)
    ["b"]=>
    int(20)
    ["c"]=>
    array(0) {
    }
  }
}
object(Closure)#%d (1) {
  ["this"]=>
  object(Foo)#%d (0) {
  }
}

