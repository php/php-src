--TEST--
json_decode() with large integers
--FILE--
<?php
$json = '{"largenum":123456789012345678901234567890}';
$x = json_decode($json);
var_dump($x->largenum);
$x = json_decode($json, false, 512, JSON_BIGINT_AS_STRING);
var_dump($x->largenum);
echo "Done\n";
?>
--EXPECT--
float(1.2345678901234568E+29)
string(30) "123456789012345678901234567890"
Done
