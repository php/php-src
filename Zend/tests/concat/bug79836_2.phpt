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
--EXPECT--
abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabc
