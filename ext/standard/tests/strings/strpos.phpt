--TEST--
Test strpos() function
--INI--
precision=14
--FILE--
<?php
echo "*** Testing basic functionality of strpos() ***\n";
var_dump( strpos("test string", "test") );
var_dump( strpos("test string", "string") );
var_dump( strpos("test string", "strin") );
var_dump( strpos("test string", "t s") );
var_dump( strpos("test string", "g") );
var_dump( strpos("te".chr(0)."st", chr(0)) );
var_dump( strpos("tEst", "test") );
var_dump( strpos("teSt", "test") );
var_dump( strpos("", "") );
var_dump( strpos("a", "") );
var_dump( strpos("", "a") );
var_dump( strpos("\\\\a", "\\a") );


echo "\n*** Testing strpos() to find various needles and a long string ***\n";
$string =
"Hello world,012033 -3.3445     NULL TRUE FALSE\0 abcd\xxyz \x000 octal\n
abcd$:Hello world";

/* needles in an array to get the position of needle in $string */
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

/* loop through to get the "needle" position in $string */
for( $i = 0; $i < count($needles); $i++ ) {
  echo "Position of '$needles[$i]' is => ";
  var_dump( strpos($string, $needles[$i]) );
}


echo "\n*** Testing strpos() with possible variations in offset ***\n";
$offset_values = array (
  1,  // offset = 1
  "string",  // offset as string, converts to zero
  "",  // offset as string, converts to zero
  "0",
  TRUE,
  FALSE,
  "string12",
  -10, // Not found
  -15, // Found
  -strlen($string),
);

/* loop through to get the "needle" position in $string */
for( $i = 0; $i < count( $offset_values ); $i++ ) {
  echo "Position of 'Hello' with offset '$offset_values[$i]' is => ";
  try {
    var_dump( strpos($string, "Hello", $offset_values[$i]) );
  } catch (TypeError $e) {
    echo "\n", $e->getMessage(), "\n";
  }
}


echo "\n*** Testing miscellaneous input data ***\n";

echo "-- Passing objects as string and needle --\n";
/* we get "Recoverable fatal error: saying Object of class needle could not be
 converted to string" by default when an object is passed instead of string:
 The error can be avoided by choosing the __toString magix method as follows: */

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

var_dump( strpos("$obj_string", "$obj_needle") );

echo "\n-- Posiibilities with null --\n";
var_dump( strpos("", NULL) );
var_dump( strpos(NULL, NULL) );
var_dump( strpos("a", NULL) );
var_dump( strpos("/x0", "0") );	 // Hexadecimal NUL

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
var_dump( strpos($string, "abcd") );
var_dump( strpos($string, "abcd", 72) );  // 72 -> "\n" in the first line
var_dump( strpos($string, "abcd", 73) );  // 73 -> "abcd" in the second line
var_dump( strpos($string, "9", (strlen($string)-1)) );

echo "\n-- A heredoc null string --\n";
$str = <<<EOD
EOD;
var_dump( strpos($str, "\0") );
var_dump( strpos($str, "0") );


echo "\n-- simple and complex syntax strings --\n";
$needle = 'world';

/* Simple syntax */
var_dump( strpos("Hello, world", "$needle") );  // works
var_dump( strpos("Hello, world'S", "$needle'S") );  // works
var_dump( strpos("Hello, worldS", "$needleS") );  // won't work

/* String with curly braces, complex syntax */
var_dump( strpos("Hello, worldS", "${needle}S") );  // works
var_dump( strpos("Hello, worldS", "{$needle}S") );  // works

echo "\n*** Testing error conditions ***\n";
var_dump( strpos($string, "") );
try {
    strpos($string, "test", strlen($string)+1);  // offset > strlen()
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strpos($string, "test", -strlen($string)-1);  // offset before start
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>

DONE
--EXPECTF--
Deprecated: Using ${var} in strings is deprecated, use {$var} instead in %s on line %d
*** Testing basic functionality of strpos() ***
int(0)
int(5)
int(5)
int(3)
int(10)
int(2)
bool(false)
bool(false)
int(0)
int(0)
bool(false)
int(1)

*** Testing strpos() to find various needles and a long string ***
Position of 'Hello world' is => int(0)
Position of 'WORLD' is => bool(false)
Position of '%0' is => int(46)
Position of '%0' is => int(46)
Position of '%00' is => int(58)
Position of 'abcd' is => int(48)
Position of 'xyz' is => int(54)
Position of 'octal' is => int(61)
Position of '-3' is => int(19)
Position of '-3' is => int(19)
Position of '-3.344' is => int(19)
Position of '-3.344' is => int(19)
Position of 'NULL' is => int(31)
Position of '0' is => int(12)
Position of '0' is => int(12)
Position of '1' is => int(13)
Position of 'TRUE' is => int(36)
Position of '1' is => int(13)
Position of '1' is => int(13)
Position of '' is => int(0)
Position of 'FALSE' is => int(41)
Position of ' ' is => int(5)
Position of '     ' is => int(26)
Position of 'b' is => int(49)
Position of '\n' is => bool(false)
Position of '
' is => int(66)
Position of '12' is => int(13)
Position of '12twelve' is => bool(false)
Position of 'Hello world,012033 -3.3445     NULL TRUE FALSE%0 abcd\xxyz %00 octal

abcd$:Hello world' is => int(0)

*** Testing strpos() with possible variations in offset ***
Position of 'Hello' with offset '1' is => int(74)
Position of 'Hello' with offset 'string' is => 
strpos(): Argument #3 ($offset) must be of type int, string given
Position of 'Hello' with offset '' is => 
strpos(): Argument #3 ($offset) must be of type int, string given
Position of 'Hello' with offset '0' is => int(0)
Position of 'Hello' with offset '1' is => int(74)
Position of 'Hello' with offset '' is => int(0)
Position of 'Hello' with offset 'string12' is => 
strpos(): Argument #3 ($offset) must be of type int, string given
Position of 'Hello' with offset '-10' is => bool(false)
Position of 'Hello' with offset '-15' is => int(74)
Position of 'Hello' with offset '-85' is => int(0)

*** Testing miscellaneous input data ***
-- Passing objects as string and needle --
int(7)

-- Posiibilities with null --

Deprecated: strpos(): Passing null to parameter #2 ($needle) of type string is deprecated in %s on line %d
int(0)

Deprecated: strpos(): Passing null to parameter #1 ($haystack) of type string is deprecated in %s on line %d

Deprecated: strpos(): Passing null to parameter #2 ($needle) of type string is deprecated in %s on line %d
int(0)

Deprecated: strpos(): Passing null to parameter #2 ($needle) of type string is deprecated in %s on line %d
int(0)
int(2)

-- A longer and heredoc string --
int(0)
int(73)
int(73)
int(728)

-- A heredoc null string --
bool(false)
bool(false)

-- simple and complex syntax strings --
int(7)
int(7)

Warning: Undefined variable $needleS in %s on line %d
int(0)
int(7)
int(7)

*** Testing error conditions ***
int(0)
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

DONE
