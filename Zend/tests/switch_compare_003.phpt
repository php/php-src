--TEST--
Test switch with duplicate string cases
--FILE--
<?php
function foo ($var) {
	switch ($var) {
		case "a":
			return "ok";
		case "a":
			return "bug";
		case "a":
			return "bug";
		default:
			return "bug";
	}
}

echo foo("a") . "\n";
?>
--EXPECT--
ok
