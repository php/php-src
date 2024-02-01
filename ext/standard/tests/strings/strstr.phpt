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
$needles = array(
  "Hello world",
  "WORLD",
  "\0",
  "\x00",
  "\x000",
  "abcd",
  "xyz",
  "octal",
  "-3",
  -3,
  "-3.344",
  -3.344,
  "NULL",
  "0",
  0,
  TRUE,
  "TRUE",
  "1",
  1,
  FALSE,
  "FALSE",
  " ",
  "     ",
  'b',
  '\n',
  "\n",
  "12",
  "12twelve",
  $string
);

/* loop through to get the string starts with "needle" in $string */
for( $i = 0; $i < count($needles); $i++ ) {
  echo "\n-- Iteration $i --\n";
  var_dump( strstr($string, $needles[$i]) );
}


echo "\n*** Testing miscellaneous input data ***\n";

echo "-- Passing objects as string and needle --\n";
/* we get "Recoverable fatal error: saying Object of class needle could not be
converted to string" by default when an object is passed instead of string:
The error can be  avoided by choosing the __toString magix method as follows: */

class StringCapable
{
  function __toString() {
    return "Hello, world";
  }
}
$obj_string = new StringCapable;

class needle
{
  function __toString() {
    return "world";
  }
}
$obj_needle = new needle;

var_dump(strstr("$obj_string", "$obj_needle"));


echo "\n-- Posiibilities with null --\n";
var_dump( strstr("", NULL) );
var_dump( strstr(NULL, NULL) );
var_dump( strstr("a", NULL) );
var_dump( strstr("/x0", "0") );  // Hexadecimal NUL

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

echo "\n-- A heredoc null string --\n";
$str = <<<EOD
EOD;
var_dump( strstr($str, "\0") );
var_dump( strstr($str, "0") );


echo "\n-- simple and complex syntax strings --\n";
$needle = 'world';

/* Simple syntax */
var_dump( strstr("Hello, world", "$needle") );  // works
var_dump( strstr("Hello, world'S", "$needle'S") );  // works
var_dump( strstr("Hello, worldS", "$needleS") );  // won't work

/* String with curly braces, complex syntax */
var_dump( strstr("Hello, worldS", "${needle}S") );  // works
var_dump( strstr("Hello, worldS", "{$needle}S") );  // works

echo "\n*** Testing error conditions ***";
var_dump( strstr($string, ""));
var_dump( strstr("a", "b", "c") );  // args > expected

?>

DONE
--EXPECTF--
Deprecated: Using ${var} in strings is deprecated, use {$var} instead in %s on line %d
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
string(39) "%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 4 --
string(27) "%00 octal

abcd$:Hello world"

-- Iteration 5 --
string(37) "abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 6 --
string(31) "xyz %00 octal

abcd$:Hello world"

-- Iteration 7 --
string(24) "octal

abcd$:Hello world"

-- Iteration 8 --
string(66) "-3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 9 --
string(66) "-3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 10 --
string(66) "-3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 11 --
string(66) "-3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 12 --
string(54) "NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 13 --
string(73) "012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 14 --
string(73) "012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 15 --
string(72) "12033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 16 --
string(49) "TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 17 --
string(72) "12033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 18 --
string(72) "12033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 19 --
string(85) "Hello world,012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 20 --
string(44) "FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 21 --
string(80) " world,012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 22 --
string(59) "     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 23 --
string(36) "bcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 24 --
bool(false)

-- Iteration 25 --
string(19) "

abcd$:Hello world"

-- Iteration 26 --
string(72) "12033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

-- Iteration 27 --
bool(false)

-- Iteration 28 --
string(85) "Hello world,012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world"

*** Testing miscellaneous input data ***
-- Passing objects as string and needle --
string(5) "world"

-- Posiibilities with null --

Deprecated: strstr(): Passing null to parameter #2 ($needle) of type string is deprecated in %s on line %d
string(0) ""

Deprecated: strstr(): Passing null to parameter #1 ($haystack) of type string is deprecated in %s on line %d

Deprecated: strstr(): Passing null to parameter #2 ($needle) of type string is deprecated in %s on line %d
string(0) ""

Deprecated: strstr(): Passing null to parameter #2 ($needle) of type string is deprecated in %s on line %d
string(1) "a"
string(1) "0"

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

-- A heredoc null string --
bool(false)
bool(false)

-- simple and complex syntax strings --
string(5) "world"
string(7) "world'S"

Warning: Undefined variable $needleS in %s on line %d
string(13) "Hello, worldS"
string(6) "worldS"
string(6) "worldS"

*** Testing error conditions ***string(729) "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"
bool(false)

DONE
