--TEST--
Bug #31177 (memory corruption because of incorrect refcounting)
--FILE--
<?php
class foo {
  function foo($n=0) {
    if($n) throw new Exception("new");
  }
}
$x = new foo();
try {
  $y=$x->foo(1);
} catch (Exception $e) {
  var_dump($x);
}
--EXPECT--
object(foo)#1 (0) {
}
