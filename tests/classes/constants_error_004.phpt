--TEST--
Class constant whose initial value references a non-existent class
--FILE--
<?php
  class C
  {
      const c1 = D::hello;
  }

  $a = new C();
?>
--EXPECTF--
Fatal error: Uncaught Error: Class 'D' not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
