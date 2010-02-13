--TEST--
Class constant whose initial value refereces a non-existent class
--FILE--
<?php
  class C
  {
      const c1 = D::hello;
  }
  
  $a = new C();
?>
--EXPECTF--
Fatal error: Undefined class constant 'D::hello' in %s on line %d
