--TEST--
Test switch with similar numeric string cases (match)
--FILE--
<?php
function foo ($var) {
	switch ($var) {
		case "12a":
			return "bug1";
		case "12b":
			return "bug2";
		case "12c":
			return "ok";
		default:
			return "bug3";
	}
	print "\n";
}

echo foo("12c") . "\n";
?>
--EXPECT--
ok
