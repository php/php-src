--TEST--n
Bug #14580 (key() not binary safe)
--FILE--
<?php
	$arr = array ("foo\0bar" => "foo\0bar");
	print_r($arr);
	$key = key($arr);
	echo strlen($key), ': ', $key, "\n";
?>
--EXPECT--
Array
(
    [foobar] => foobar
)
7: foobar
