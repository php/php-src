--TEST--
Bug #64578 (debug_backtrace in set_error_handler corrupts zend heap: segfault)
--FILE--
<?php
function x($s) { 
	$resource = fopen("php://input", "r"); 
	$s[$resource] = '2';
}
$y = "1";
x($y);
var_dump($y);
?>
--EXPECTF--
Warning: Illegal offset type in %sbug64578.php on line %d
string(1) "1"
