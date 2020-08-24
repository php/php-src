--TEST--
Bug #79979 (passing value to by-ref param via CUF(A) crashes)
--FILE--
<?php
call_user_func_array("str_replace", ["a", "b", "c", 0]);

$cufa = "call_user_func_array";
$cufa("str_replace", ["a", "b", "c", 0]);

call_user_func_array($cufa, ["str_replace", ["a", "b", "c", 0]]);
?>
--EXPECTF--
Warning: str_replace(): Argument #4 ($replace_count) must be passed by reference, value given in %s on line %d

Warning: str_replace(): Argument #4 ($replace_count) must be passed by reference, value given in %s on line %d

Warning: str_replace(): Argument #4 ($replace_count) must be passed by reference, value given in %s on line %d
