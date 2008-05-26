--TEST--
bitwise AND and strings
--FILE--
<?php

$s = b"123";
$s1 = b"234";

var_dump($s & $s1);

$s = b"test";
$s1 = b"some";

var_dump($s & $s1);

$s = b"test long";
$s1 = b"some";

var_dump($s & $s1);

$s = b"test";
$s1 = b"some long";

var_dump($s & $s1);

$s = b"test";
$s &= b"some long";

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
