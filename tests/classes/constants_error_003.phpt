--TEST--
Basic class support - attempting to pass a class constant by reference.
--FILE--
<?php
  class aclass
  {
      const myConst = "hello";
  }

  function f(&$a)
  {
      $a = "changed";
  }

  f(aclass::myConst);
  var_dump(aclass::myConst);
?>
--EXPECTF--
Fatal error: Uncaught Error: f(): Argument #1 ($a) cannot be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
