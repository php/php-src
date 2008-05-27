--TEST--
Test sprintf() function : usage variations - with whitespaces in format strings
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : with  white spaces in format strings ***\n";

// initializing the format array
$formats = array(
  "% d", "%  d", "%   d",
  "% f", "%  f", "%   f",
  "% F", "%  F", "%   F",
  "% b", "%  b", "%   b",
  "% c", "%  c", "%   c",
  "% e", "%  e", "%   e",
  "% u", "%  u", "%   u",
  "% o", "%  o", "%   o",
  "% x", "%  x", "%   x",
  "% X", "%  X", "%   X",
  "% E", "%  E", "%   E"
);

// initializing the args array

foreach($formats as $format) {
  var_dump( sprintf($format, 1234) );
}

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : with  white spaces in format strings ***
unicode(4) "1234"
unicode(4) "1234"
unicode(4) "1234"
unicode(11) "1234.000000"
unicode(11) "1234.000000"
unicode(11) "1234.000000"
unicode(11) "1234.000000"
unicode(11) "1234.000000"
unicode(11) "1234.000000"
unicode(11) "10011010010"
unicode(11) "10011010010"
unicode(11) "10011010010"
unicode(1) "ￒ"
unicode(1) "ￒ"
unicode(1) "ￒ"
unicode(11) "1.234000e+3"
unicode(11) "1.234000e+3"
unicode(11) "1.234000e+3"
unicode(4) "1234"
unicode(4) "1234"
unicode(4) "1234"
unicode(4) "2322"
unicode(4) "2322"
unicode(4) "2322"
unicode(3) "4d2"
unicode(3) "4d2"
unicode(3) "4d2"
unicode(3) "4D2"
unicode(3) "4D2"
unicode(3) "4D2"
unicode(11) "1.234000E+3"
unicode(11) "1.234000E+3"
unicode(11) "1.234000E+3"
Done
