--TEST--
Closure application fuzz 003
--FILE--
<?php
class Foo {
    function __call($name, $args) {
        var_dump($args);
    }
}
$foo = new Foo;
$bar = $foo->method(1, ...);
$bar(2);
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
