--TEST--
Test printf() function : basic functionality - bool format
--FILE--
<?php
echo "*** Testing printf() : basic functionality - using bool format ***\n";


// Initialise all required variables
$format = "format";
$format1 = "%b";
$format2 = "%b %b";
$format3 = "%b %b %b";
$arg1 = TRUE;
$arg2 = FALSE;
$arg3 = true;

echo "\n-- Calling printf() with no arguments --\n";
$result = printf($format);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with one arguments--\n";
$result = printf($format1, $arg1);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with two arguments--\n";
$result = printf($format2, $arg1, $arg2);
echo "\n";
var_dump($result);

echo "\n-- Calling printf() with three arguments--\n";
$result = printf($format3, $arg1, $arg2, $arg3);
echo "\n";
var_dump($result);
?>
--EXPECT--
*** Testing printf() : basic functionality - using bool format ***

-- Calling printf() with no arguments --
format
int(6)

-- Calling printf() with one arguments--
1
int(1)

-- Calling printf() with two arguments--
1 0
int(3)

-- Calling printf() with three arguments--
1 0 1
int(5)
