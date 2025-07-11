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
"Hello world,012033 -3.3445     NULL TRUE FALSE\0 abcd\xxyz \x000 octal\nabcd$:Hello world";

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
  "-3.344",
  "NULL",
  "0",
  "TRUE",
  "1",
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
foreach ($needles as $needle) {
  echo "Position of '$needle' is => ";
  var_dump( strpos($string, $needle) );
}

echo "\n*** Testing strpos() with possible variations in offset ***\n";
$offsets = [
    1,
    0,
    -10, // Not found
    -15, // Found
    -strlen($string),
];

/* loop through to get the "needle" position in $string */
foreach ($offsets as $offset) {
  echo "Position of 'Hello' with offset $offset is => ";
  try {
    var_dump( strpos($string, "Hello", $offset) );
  } catch (TypeError $e) {
    echo "\n", $e->getMessage(), "\n";
  }
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
var_dump( strpos($string, "abcd") );
var_dump( strpos($string, "abcd", 72) );  // 72 -> "\n" in the first line
var_dump( strpos($string, "abcd", 73) );  // 73 -> "abcd" in the second line
var_dump( strpos($string, "9", (strlen($string)-1)) );

?>
--EXPECTF--
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
Position of '-3.344' is => int(19)
Position of 'NULL' is => int(31)
Position of '0' is => int(12)
Position of 'TRUE' is => int(36)
Position of '1' is => int(13)
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
Position of 'Hello' with offset 1 is => int(73)
Position of 'Hello' with offset 0 is => int(0)
Position of 'Hello' with offset -10 is => bool(false)
Position of 'Hello' with offset -15 is => int(73)
Position of 'Hello' with offset -84 is => int(0)

-- A longer and heredoc string --
int(0)
int(73)
int(73)
int(728)
