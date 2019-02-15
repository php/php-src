--TEST--
bitwise AND and strings
--FILE--
<?php

$s = "123";
$s1 = "234";

var_dump($s & $s1);

$s = "test";
$s1 = "some";

var_dump($s & $s1);

$s = "test long";
$s1 = "some";

var_dump($s & $s1);

$s = "test";
$s1 = "some long";

var_dump($s & $s1);

$s = "test";
$s &= "some long";

var_dump($s);

echo "Done\n";
?>
--EXPECT--
string(3) "020"
string(4) "pead"
string(4) "pead"
string(4) "pead"
string(4) "pead"
Done
