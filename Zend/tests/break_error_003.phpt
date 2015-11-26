--TEST--
'break' error (not in the loop context)
--FILE--
<?php
function foo () {
	break;
}
?>
--EXPECTF--
Fatal error: 'break' not in the 'loop' or 'switch' context in %sbreak_error_003.php on line 3
