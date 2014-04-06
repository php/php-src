--TEST--
Basic return hints at execution allowing null
--FILE--
<?php
function test1() : ?array {
	return null;
}

var_dump(test1());
?>
--EXPECT--
NULL

