--TEST--
Bug #79979 (passing value to by-ref param via CUF(A) crashes)
--FILE--
<?php
call_user_func_array('exec', ['echo foo', '', '']);

$cufa = 'call_user_func_array';
$cufa('exec', ['echo foo', '', '']);

call_user_func_array($cufa, ['exec', ['echo foo', '', '']]);
?>
--EXPECTF--
Warning: Parameter 2 to exec() expected to be a reference, value given in %s on line %d

Warning: Parameter 3 to exec() expected to be a reference, value given in %s on line %d

Warning: Parameter 2 to exec() expected to be a reference, value given in %s on line %d

Warning: Parameter 3 to exec() expected to be a reference, value given in %s on line %d

Warning: Parameter 2 to exec() expected to be a reference, value given in %s on line %d

Warning: Parameter 3 to exec() expected to be a reference, value given in %s on line %d
