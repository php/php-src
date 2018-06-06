--TEST--
Bug #14580 (key() not binary safe)
--FILE--
<?php
	$arr = array ("foo\0bar" => "foo\0bar");
	$key = key($arr);
	echo strlen($key), ': ';
	echo urlencode($key), "\n";
?>
--EXPECT--
7: foo%00bar
