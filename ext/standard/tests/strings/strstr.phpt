--TEST--
Test strstr() function
--FILE--
<?php
echo "*** Testing basic functionality of strstr() ***\n";
var_dump( strstr("test string", "test") );
var_dump( strstr("test string", "string") );
var_dump( strstr("test string", "strin") );
var_dump( strstr("test string", "t s") );
var_dump( strstr("test string", "g") );
var_dump( md5(strstr("te".chr(0)."st", chr(0))) );
var_dump( strstr("tEst", "test") );
var_dump( strstr("teSt", "test") );
var_dump( strstr("", "") );
var_dump( strstr("a", "") );
var_dump( strstr("", "a") );


echo "\n*** Testing strstr() with various needles ***";
$string =
"Hello world,012033 -3.3445     NULL TRUE FALSE\0 abcd\xxyz \x000 octal\n
abcd$:Hello world";

/* needles in an array to get the string starts with needle, in $string */
$needles = [
    "Hello world",
    "WORLD",
    "\0",		// Null
    "-3",
    '-3.344',
    "NULL",
    "0",
    "TRUE",
    "1",
    "FALSE",
    " ",
    "     ",
    'b',		// single char
    "\t",		// escape sequences
    "12",
    "12twelve", // int + string
];

/* loop through to get the string starts with "needle" in $string */
for( $i = 0; $i < count($needles); $i++ ) {
  echo "\n-- Iteration $i --\n";
  var_dump( strstr($string, $needles[$i]) );
}

echo "\n-- A longer and heredoc string --\n";
$string = <<<EOD
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
EOD;
var_dump( strstr($string, "abcd") );
var_dump( strstr($string, "1234") );

?>

DONE
--EXPECTF--
*** Testing basic functionality of strstr() ***
string(11) "test string"
string(6) "string"
string(6) "string"
string(8) "t string"
string(1) "g"
string(32) "7272696018bdeb2c9a3f8d01fc2a9273"
bool(false)
bool(false)
string(0) ""
string(1) "a"
bool(false)

*** Testing strstr() with various needles ***
-- Iteration 0 --
string(85) "Hello world,012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 1 --
bool(false)

-- Iteration 2 --
string(39) "%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 3 --
string(66) "-3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 4 --
string(66) "-3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 5 --
string(54) "NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 6 --
string(73) "012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 7 --
string(49) "TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 8 --
string(72) "12033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 9 --
string(44) "FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 10 --
string(80) " world,012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 11 --
string(59) "     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 12 --
string(36) "bcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 13 --
bool(false)

-- Iteration 14 --
string(72) "12033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 15 --
bool(false)

-- A longer and heredoc string --
string(729) "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
string(702) "123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"

DONE
