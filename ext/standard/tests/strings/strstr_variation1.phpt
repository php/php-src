--TEST--
Test strstr() function : usage variations - complex strings containing other than 7-bit chars
--FILE--
<?php
$string = chr(0).chr(128).chr(129).chr(234).chr(235).chr(254).chr(255);
$stringAsHex = bin2hex($string);
echo "-- Positions of some chars in the string '$stringAsHex' are as follows --\n";
echo bin2hex( chr(128) ) ." => ";
var_dump( bin2hex( strstr($string, chr(128) ) ) );
echo bin2hex( chr(255) ) ." => ";
var_dump( bin2hex( strstr($string, chr(255) ) ) );
echo bin2hex( chr(256) ) ." => ";
var_dump( bin2hex( strstr($string, chr(256) ) ) );
?>

DONE
--EXPECT--
-- Positions of some chars in the string '008081eaebfeff' are as follows --
80 => string(12) "8081eaebfeff"
ff => string(2) "ff"
00 => string(14) "008081eaebfeff"

DONE
