--TEST--
GH-14775: Range negative step overflow
--FILE--
<?php
$var = -PHP_INT_MAX - 1;
try {
	range($var,1,$var);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
--EXPECTF--
ValueError: range(): Argument #3 ($step) must be greater than %s
