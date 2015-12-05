--TEST--
Class constant whose initial value references a non-existent class
--FILE--
<?php
  class C
  {
      const c1 = D::hello;
  }

  var_dump(C::c1);
?>
--EXPECTF--
Fatal error: Class 'D' not found in %s on line %d
