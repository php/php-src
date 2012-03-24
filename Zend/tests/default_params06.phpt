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
int(7)
int(2)
int(7)
int(2)
Done