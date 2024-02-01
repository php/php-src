--TEST--
Bug #55305 (ref lost: 1st ref instantiated in class def, 2nd ref made w/o instantiating)
--FILE--
<?php
#[AllowDynamicProperties]
class Foo {
  var $foo = "test";
}

$f = new Foo();
$f->bar =& $f->foo;
var_dump($f->foo);
var_dump($f->bar);
?>
--EXPECT--
string(4) "test"
string(4) "test"
