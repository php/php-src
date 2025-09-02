--TEST--
Bug #80742 (Opcache JIT makes some boolean logic unexpectedly be true)
--EXTENSIONS--
opcache
--FILE--
<?php

function checkGroundState(float $movY, float $dy) : void{
	var_dump($movY != $dy, $movY < 0, ($movY != $dy and $movY < 0));
	var_dump("wow!");
}

checkGroundState(0, 0);

function eq(float $a, float $b, $c, $d) {
	if ($a == $b) {
		echo 1;
	}
	if ($a == $b) {
	} else {
		echo 2;
	}
	if ($a != $b) {
		echo 3;
	}
	if ($a != $b) {
	} else {
		echo 4;
	}
	if ($a === $b) {
		echo 5;
	}
	if ($a === $b) {
	} else {
		echo 6;
	}
	if ($a !== $b) {
		echo 7;
	}
	if ($a !== $b) {
	} else {
		echo 8;
	}
	echo "\n";
	var_dump(
		$a == $b && $c,
		$a != $b && $c,
		$a === $b && $c,
		$a !== $b && $c,);
	var_dump(
		$a == $b || $d,
		$a != $b || $d,
		$a === $b || $d,
		$a !== $b || $d);
}
eq(3.0, 3.0, true, false);
eq(3.0,	2.0, true, false);
eq(3.0, NAN, true, false);
eq(NAN, NAN, true, false);
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
string(4) "wow!"
1458
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
2367
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
2367
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
2367
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
