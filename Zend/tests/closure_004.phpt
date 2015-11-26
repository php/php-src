--TEST--
Closure 004: Lambda with lexical variables (scope lifetime)
--FILE--
<?php

function run () {
	$x = 4;

	$lambda1 = function () use ($x) {
		echo "$x\n";
	};

	$lambda2 = function () use (&$x) {
		echo "$x\n";
		$x++;
	};

	return array($lambda1, $lambda2);
}

list ($lambda1, $lambda2) = run();

$lambda1();
$lambda2();
$lambda1();
$lambda2();

echo "Done\n";
?>
--EXPECT--
4
4
4
5
Done
