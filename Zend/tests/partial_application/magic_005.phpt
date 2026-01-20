--TEST--
PFA magic null ptr deref in arginfo
--FILE--
<?php
class Foo {
    function __call($name, $args) {
    }
}
$foo = new Foo;
$bar = $foo->method(?);
var_dump($bar);
?>
--EXPECTF--
object(Closure)#%d (%d) {
  ["name"]=>
  string(%d) "{closure:%s}"
  ["file"]=>
  string(%d) "%smagic_005.php"
  ["line"]=>
  int(8)
  ["this"]=>
  object(Foo)#%d (0) {
  }
  ["parameter"]=>
  array(1) {
    ["$arguments0"]=>
    string(10) "<required>"
  }
}
