--TEST--
Default parameters - 6, by-refs
--FILE--
<?php
function test($a=1, &$b=2) {
	var_dump($a, $b);
	$b = 10;
}

test(7);
test(7, default);
echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot pass parameter 2 by reference in %s on line %d
