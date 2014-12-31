--TEST--
Try finally (segfault with empty break)
--FILE--
<?php
function foo () {
	try {
		break;
	} finally {
	}
}

foo();
?>
--EXPECTF--
Fatal error: Cannot break/continue 1 level in %stry_finally_011.php on line %d
