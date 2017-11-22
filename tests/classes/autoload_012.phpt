--TEST--
Ensure callback methods in unknown classes trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});
call_user_func("UndefC::test");
?>
--EXPECTF--
In autoload: string(6) "UndefC"

Warning: call_user_func() expects parameter 1 to be a valid callback, class 'UndefC' not found in %s on line %d
