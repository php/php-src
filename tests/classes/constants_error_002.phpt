--TEST--
Error case: class constant as an array
--FILE--
<?php
  class myclass
  {
      const myConst = array();
  }
?>
--EXPECTF--

Fatal error: Arrays are not allowed in class constants in %s on line 4
