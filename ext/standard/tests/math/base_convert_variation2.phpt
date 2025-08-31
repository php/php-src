--TEST--
Test base_convert() function : strange literals
--FILE--
<?php
echo 'Binary to decimal:', \PHP_EOL;
var_dump(base_convert('0b', 2, 10));
var_dump(base_convert('0B', 2, 10));
var_dump(base_convert('', 2, 10));
echo 'Octal to decimal:', \PHP_EOL;
var_dump(base_convert('0o', 8, 10));
var_dump(base_convert('0O', 8, 10));
var_dump(base_convert('0', 8, 10));
var_dump(base_convert('', 8, 10));
echo 'Hexadecimal to decimal:', \PHP_EOL;
var_dump(base_convert('0x', 16, 10));
var_dump(base_convert('0X', 16, 10));
var_dump(base_convert('', 16, 10));
?>
--EXPECT--
Binary to decimal:
string(1) "0"
string(1) "0"
string(1) "0"
Octal to decimal:
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
Hexadecimal to decimal:
string(1) "0"
string(1) "0"
string(1) "0"
