--TEST--
Test strpos() function : usage variations - complex strings containing other than 7-bit chars
--FILE--
<?php
$string = chr(0).chr(128).chr(129).chr(234).chr(235).chr(254).chr(255);
$stringAsHex = bin2hex($string);
echo "-- Positions of some chars in the string '$stringAsHex' are as follows --\n";
echo bin2hex( chr(128) ) ." => ";
var_dump( strpos($string, chr(128)) );
echo bin2hex( chr(255) ) ." => ";
var_dump( strpos($string, chr(255), 3) );
echo bin2hex( chr(256) ) ." => ";
var_dump( strpos($string, chr(256)) );
?>

DONE
--EXPECT--
-- Positions of some chars in the string '008081eaebfeff' are as follows --
80 => int(1)
ff => int(6)
00 => int(0)

DONE
