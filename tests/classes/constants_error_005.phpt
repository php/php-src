--TEST--
Error case: class constant as an encapsed containing a variable
--FILE--
<?php
  class myclass
  {
      const myConst = "$myVar";
  }
?>
--EXPECTF--

Parse error: %s in %s on line %d
