--TEST--
Bug #31177 (memory corruption because of incorrect refcounting)
--FILE--
<?php
class foo {
  function __construct($n=0) {
    if($n) throw new Exception("new");
  }
  function another() {
    return self::__construct(1);
  }
}
$x = new foo();
try {
  $y=$x->another();
} catch (Exception $e) {
  var_dump($x);
}
--EXPECT--
object(foo)#1 (0) {
}
