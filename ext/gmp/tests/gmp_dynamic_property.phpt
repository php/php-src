--TEST--
GH-9186 Dynamic property unserialization should trigger a deprecated notice
--EXTENSIONS--
gmp
--FILE--
<?php

$g = gmp_init(0);
$g->{1} = 123;

$serialized = serialize($g);
var_dump(unserialize($serialized));

?>
--EXPECTF--
Deprecated: Creation of dynamic property GMP::$1 is deprecated in %s on line %d

Deprecated: Creation of dynamic property GMP::$1 is deprecated in %s on line %d
object(GMP)#%d (%d) {
  [1]=>
  int(123)
  ["num"]=>
  string(1) "0"
}
