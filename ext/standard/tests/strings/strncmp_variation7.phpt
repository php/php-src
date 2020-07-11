--TEST--
Test strncmp() function : usage variations - binary safe(null terminated strings)
--FILE--
<?php
/* Test strncmp() function with binary values passed to 'str1' & 'str2' and with the null terminated strings */

echo "*** Test strncmp() function: Checking with the null terminated strings ***\n";

/* A binary function should not expect a null terminated string, and it should treat input as a raw stream of data */
$str1 = "Hello\0world";
$str2 = "Hello\0";
var_dump( strncmp($str1, $str2, 12) );  //expected: int(5);

echo "*** Done ***\n";
?>
--EXPECT--
*** Test strncmp() function: Checking with the null terminated strings ***
int(5)
*** Done ***
