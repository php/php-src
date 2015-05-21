--TEST--
Test switch with similar numeric string cases (no match)
--FILE--
<?php
function foo ($var) {
	switch ($var) {
		case "12a":
			return "bug1";
		case "12b":
			return "bug2";
		case "12c":
			return "bug3";
		default:
			return "ok";
	}
	print "\n";
}

echo foo("12d") . "\n";
?>
--EXPECT--
ok
