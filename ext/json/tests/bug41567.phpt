--TEST--
Bug #41567 (json_encode() double conversion is inconsistent with PHP)
--SKIPIF--
if (!extension_loaded('json')) die('skip');
--FILE--
<?php

$a = json_encode(123456789.12345);
var_dump(json_decode($a));

echo "Done\n";
?>
--EXPECT--	
float(123456789.12345)
Done
