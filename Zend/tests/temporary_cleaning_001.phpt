--TEST--
Temporary leak on exception
--FILE--
<?php

function ops() {
    throw new Exception();
}

try {
	$x = 2;
	$y = new stdClass;
	while ($x-- && new stdClass) {
		$r = [$x] + ($y ? ((array) $x) + [2] : ops());
		$y = (array) $y;
	}
} catch (Exception $e) {
}

?>
==DONE==
--EXPECT--
==DONE==
