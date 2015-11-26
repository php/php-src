--TEST--
'break' error (wrong level)
--FILE--
<?php
function foo () {
	while (1) {
		break 2;
	}
}
?>
--EXPECTF--
Fatal error: Cannot 'break' 2 levels in %sbreak_error_004.php on line 4
