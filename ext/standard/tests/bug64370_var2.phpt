--TEST--
Test bug #64370 sequential microtime(true) calls
--FILE--
<?php

$i = 0;
while(100000 > $i++) {
	$m0 = microtime(true);
	$m1 = microtime(true);
	$d = $m1 - $m0;

	/*echo "$d\n";*/

	if ($d < 0) {
		die("failed in {$i}th iteration");
	}
}
echo "ok\n";
?>
===DONE===
--EXPECT--
ok
===DONE===
