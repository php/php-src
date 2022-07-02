--TEST--
Test strncmp() function: usage variations - different lengths
--FILE--
<?php
/* Test strncmp() with various lengths */

echo "*** Test strncmp() function: with different lengths ***\n";
/* definitions of required variables */
$str1 = "Hello, World\n";
$str2 = "Hello, world\n";

/* loop through to compare the strings, for various length values */
for($len = strlen($str1); $len >= 0; $len--) {
  var_dump( strncmp($str1, $str2, $len) );
}
echo "*** Done ***\n";
?>
--EXPECTREGEX--
\*\*\* Test strncmp\(\) function: with different lengths \*\*\*
int\(-[1-9][0-9]*\)
int\(-[1-9][0-9]*\)
int\(-[1-9][0-9]*\)
int\(-[1-9][0-9]*\)
int\(-[1-9][0-9]*\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(0\)
int\(0\)
int\(0\)
int\(0\)
int\(0\)
int\(0\)
int\(0\)
\*\*\* Done \*\*\*
