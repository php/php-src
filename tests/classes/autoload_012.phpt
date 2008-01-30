--TEST--
Ensure callback methods in unknown classes trigger autoload.
--FILE--
<?php
  function __autoload($name)
  {
      echo "In autoload: ";
      var_dump($name);
  }
  call_user_func("UndefC::test");
?>
--EXPECTF--
In autoload: string(6) "UndefC"

Warning: call_user_func(UndefC::test): First argument is expected to be a valid callback in %s on line %d

