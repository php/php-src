--TEST--
Bug #24783 ($key not binary safe in "foreach($arr as $key => $val)")
--FILE--
<?php
error_reporting(E_ALL);
	$arr = array ("foo\0bar" => "foo\0bar");
	foreach ($arr as $key => $val) {
		echo strlen($key), ": $key => $val\n";
	}
?>
--EXPECT--
7: foobar => foobar
