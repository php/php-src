--TEST--
Test strncasecmp() function : usage variations - binary safe
--FILE--
<?php
/* Test strncasecmp() function with null terminated strings and binary values passed to 'str1' & 'str2' */

echo "*** Test strncasecmp() function: with null terminated strings and binary inputs ***\n";

/* A binary function should not expect a null terminated string, and it should treat input as a raw stream of data */
$str1 = "Hello\0world";
$str2 = "Hello\0";
$str3 = "Hello,".chr(0)."world";
var_dump( strncasecmp($str1, $str2, 12) );
var_dump( strncasecmp($str3, "Hello,world", 12) );

echo "*** Done ***\n";
?>
--EXPECT--
*** Test strncasecmp() function: with null terminated strings and binary inputs ***
int(1)
int(-119)
*** Done ***
