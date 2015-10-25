--TEST--
Bug #70783 (Adding a trait to a class produces fatal error)
--FILE--
<?php
class B extends A {}
trait wtf {}
class A {
  use wtf;
}
echo "OK";
--EXPECTF--
OK
