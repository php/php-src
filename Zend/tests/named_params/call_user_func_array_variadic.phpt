--TEST--
call_user_func_array() with extra named parameters
--FILE--
<?php
$args = array("" => 1);
call_user_func_array("array_multisort", $args);
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: array_multisort() expects at least 1 argument, 0 given in %scall_user_func_array_variadic.php:3
Stack trace:
#0 %scall_user_func_array_variadic.php(3): array_multisort(: 1)
#1 {main}
  thrown in %scall_user_func_array_variadic.php on line 3
