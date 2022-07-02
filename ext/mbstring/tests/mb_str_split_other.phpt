--TEST--
mb_str_split() tests for more text encodings
--EXTENSIONS--
mbstring
--FILE--
<?php
$array = mb_str_split("\x00\x01\x02\x03\x04\x05\x06\x07", 2, "UCS-2BE");
echo "[", bin2hex($array[0]), ", ", bin2hex($array[1]), "]\n";
?>
--EXPECT--
[00010203, 04050607]
