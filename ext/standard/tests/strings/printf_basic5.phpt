--TEST--
Test printf() function : basic functionality - char format
--FILE--
<?php
echo "*** Testing printf() : basic functionality - using char format ***\n";


// Initialise all required variables
$format = "format";
$format1 = "%c";
$format2 = "%c %c";
$format3 = "%c %c %c";
$arg1 = 65;
$arg2 = 66;
$arg3 = 67;

echo "\n-- Calling printf() with no arguments --\n";
$result = printf($format);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with one arguments --\n";
$result = printf($format1, $arg1);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with two arguments --\n";
$result = printf($format2, $arg1, $arg2);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with three arguments --\n";
$result = printf($format3, $arg1, $arg2, $arg3);
echo "\n";
var_dump($result);
?>
--EXPECT--
*** Testing printf() : basic functionality - using char format ***

-- Calling printf() with no arguments --
format
int(6)

-- Calling printf() with one arguments --
A
int(1)

-- Calling printf() with two arguments --
A B
int(3)

-- Calling printf() with three arguments --
A B C
int(5)
