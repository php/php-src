--TEST--
'break' error (non positive numbers)
--FILE--
<?php
function foo () {
	break 0;
}
?>
--EXPECTF--
Fatal error: 'break' operator accepts only positive numbers in %sbreak_error_001.php on line 3
