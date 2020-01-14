--TEST--
Long salts for BCRYPT in crypt()
--FILE--
<?php

echo "2a" . \PHP_EOL;
$b = str_repeat("A", 124);
var_dump(crypt("A", "$2a$" . $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", "$2a$" . $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", "$2a$" . $b));

echo "2x" . \PHP_EOL;
$b = str_repeat("A", 124);
var_dump(crypt("A", "$2x$" . $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", "$2x$" . $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", "$2x$" . $b));

echo "2y" . \PHP_EOL;
$b = str_repeat("A", 124);
var_dump(crypt("A", "$2y$" . $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", "$2y$" . $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", "$2y$" . $b));
?>
--EXPECT--
2a
string(2) "*0"
string(2) "*0"
string(2) "*0"
2x
string(2) "*0"
string(2) "*0"
string(2) "*0"
2y
string(2) "*0"
string(2) "*0"
string(2) "*0"
