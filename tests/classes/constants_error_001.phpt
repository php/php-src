--TEST--
Error case: duplicate class constant definition
--FILE--
<?php
  class myclass
  {
      const myConst = "hello";
      const myConst = "hello again";
  }
?>
--EXPECTF--
Fatal error: Constant myclass::myConst cannot be redefined in %s on line %d
