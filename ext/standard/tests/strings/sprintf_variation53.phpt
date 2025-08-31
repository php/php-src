--TEST--
Test sprintf() function : usage variations - with whitespaces in format strings
--FILE--
<?php
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
string(4) "1234"
string(4) "1234"
string(4) "1234"
string(11) "1234.000000"
string(11) "1234.000000"
string(11) "1234.000000"
string(11) "1234.000000"
string(11) "1234.000000"
string(11) "1234.000000"
string(11) "10011010010"
string(11) "10011010010"
string(11) "10011010010"
string(1) "Ò"
string(1) "Ò"
string(1) "Ò"
string(11) "1.234000e+3"
string(11) "1.234000e+3"
string(11) "1.234000e+3"
string(4) "1234"
string(4) "1234"
string(4) "1234"
string(4) "2322"
string(4) "2322"
string(4) "2322"
string(3) "4d2"
string(3) "4d2"
string(3) "4d2"
string(3) "4D2"
string(3) "4D2"
string(3) "4D2"
string(11) "1.234000E+3"
string(11) "1.234000E+3"
string(11) "1.234000E+3"
Done
