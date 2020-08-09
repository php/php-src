--TEST--
Bug #61052 (missing error check in trait 'insteadof' clause)
--FILE--
<?php
trait T1 {
  function foo(){ echo "T1\n"; }
}
trait T2 {
  function foo(){ echo "T2\n"; }
}
class C {
  use T1, T2 {
    T1::foo insteadof T1;
  }
}
C::foo();
?>
--EXPECTF--
Fatal error: Inconsistent insteadof definition. The method foo is to be used from T1, but T1 is also on the exclude list in %s on line %d
