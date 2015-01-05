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
--EXPECT--
7: foo%00bar => foo%00bar
