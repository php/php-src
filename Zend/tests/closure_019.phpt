--TEST--
Closure 019: Calling lambda using $GLOBALS and global $var
--FILE--
<?php

$lambda = function &(&$x) {
	return $x = $x * $x;
};

function test() {
	global $lambda;
	
	$y = 3;
	var_dump($GLOBALS['lambda']($y));
	var_dump($lambda($y));
	var_dump($GLOBALS['lambda'](1));
}

test();

?>
--EXPECTF--
int(9)
int(81)

Fatal error: Cannot pass parameter 1 by reference in %s on line %d
