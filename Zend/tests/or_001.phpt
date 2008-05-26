--TEST--
bitwise OR and strings
--FILE--
<?php

$s = b"323423";
$s1 = b"2323.555";

var_dump($s | $s1);
var_dump($s1 | $s);

$s = b"some";
$s1 = b"test";

var_dump($s | $s1);

$s = b"some";
$s |= b"test";

var_dump($s);

echo "Done\n";
?>
--EXPECT--
string(8) "3337>755"
string(8) "3337>755"
string(4) "wou"
string(4) "wou"
Done
