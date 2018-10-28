--TEST--
Binary safety of each() for both keys and values
--FILE--
<?php
error_reporting(E_ALL);
$arr = array ("foo\0bar" => "foo\0bar");
while (list($key, $val) = each($arr)) {
	echo strlen($key), ': ';
	echo urlencode($key), ' => ', urlencode($val), "\n";
}
?>
--EXPECTF--
Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d
7: foo%00bar => foo%00bar
