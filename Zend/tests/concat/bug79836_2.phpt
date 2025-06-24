--TEST--
Bug #79836 (Segfault in concat_function)
--FILE--
<?php
$c = str_repeat("abcd", 10);

ob_start(function () use (&$c) {
	$c = 0;
}, 1);

class X {
	function __toString() {
		echo "a";
		return "abc";
	}
}

$xxx = new X;

$x = $c . $xxx;
ob_end_clean();
echo $x . "\n";
?>
--EXPECTF--
Deprecated: X::__toString(): Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d

Deprecated: ob_end_clean(): Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d
abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabc
