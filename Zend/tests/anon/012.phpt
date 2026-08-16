--TEST--
Ensure correct unmangling of private property names for anonymous class instances
--FILE--
<?php
var_dump(new class { private $foo; });
?>
--EXPECTF--
object(class@anonymous%0%s:2$%x)#1 (1) {
  ["foo":"class@anonymous":private]=>
  NULL
}
