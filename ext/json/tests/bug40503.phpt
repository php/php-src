--TEST--
Bug #40503 (json_encode() value corruption on 32bit systems with overflown values)
--INI--
precision=14
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
function json_test_show_eq($x, $y) {
	echo "$x ". ( $x == $y ? "==" : "!=") ." $y\n";
}

$value = 0x7FFFFFFF; #2147483647;
json_test_show_eq("$value", json_encode($value));
$value++;
json_test_show_eq("$value", json_encode($value));

?>
--EXPECT--
2147483647 == 2147483647
2147483648 == 2147483648
