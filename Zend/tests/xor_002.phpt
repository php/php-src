--TEST--
XORing strings
--FILE--
<?php

$s = b"123";
$s1 = b"234";
var_dump(bin2hex($s ^ $s1));

$s = b"1235";
$s1 = b"234";
var_dump(bin2hex($s ^ $s1));

$s = b"some";
$s1 = b"test";
var_dump(bin2hex($s ^ $s1));

$s = b"some long";
$s1 = b"test";
var_dump(bin2hex($s ^ $s1));

$s = b"some";
$s1 = b"test long";
var_dump(bin2hex($s ^ $s1));

$s = b"some";
$s ^= b"test long";
var_dump(bin2hex($s));

echo "Done\n";
?>
--EXPECT--
unicode(6) "030107"
unicode(6) "030107"
unicode(8) "070a1e11"
unicode(8) "070a1e11"
unicode(8) "070a1e11"
unicode(8) "070a1e11"
Done
