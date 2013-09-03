--TEST--
Default parameters - 7, by-refs
--FILE--
<?php

test(7);
test(7, default);
echo "Done\n";
function test($a=1, &$b=2) {
	var_dump($a, $b);
	$b = 10;
}
?>
--EXPECTF--	
int(7)
int(2)

Fatal error: Cannot pass parameter 2 by reference in %s on line %d

