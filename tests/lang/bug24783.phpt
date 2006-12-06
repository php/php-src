--TEST--
Bug #24783 ($key not binary safe in "foreach($arr as $key => $val)")
--FILE--
<?php
error_reporting(E_ALL);
	$arr = array (b"foo\0bar" => b"foo\0bar");
	foreach ($arr as $key => $val) {
		echo strlen($key), ': ';
		echo urlencode($key), ' => ', urlencode($val), "\n";
	}
?>
--EXPECT--
7: foo%00bar => foo%00bar
