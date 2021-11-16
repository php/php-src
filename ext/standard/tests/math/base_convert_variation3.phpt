--TEST--
Test base_convert() function: converting '0'
--FILE--
<?php
echo 'Binary to decimal:', \PHP_EOL;
var_dump(base_convert('0', 2, 10));
echo 'Octal to decimal:', \PHP_EOL;
var_dump(base_convert('0', 8, 10));
echo 'Hexadecimal to decimal:', \PHP_EOL;
var_dump(base_convert('0', 16, 10));
?>
--EXPECT--
Binary to decimal:
string(1) "0"
Octal to decimal:
string(1) "0"
Hexadecimal to decimal:
string(1) "0"
