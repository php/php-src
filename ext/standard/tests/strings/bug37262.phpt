--TEST--
Bug #37262 (var_export() does not escape \0 character)
--FILE--
<?php
$func = create_function('$a', 'return $a;');
var_export($func);
?>
--EXPECTF--	
'' . "\0" . 'lambda_%d'
