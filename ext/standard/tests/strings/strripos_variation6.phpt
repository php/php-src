--TEST--
Test strrpos() function : usage variations - negative offset with empty needle
--FILE--
<?php
$haystack = "Hello,\t\n\0\n  $&!#%()*<=>?@hello123456he \x234 \101 ";

var_dump(strlen($haystack));

var_dump( strripos($haystack, "", -1) );
var_dump( strripos($haystack, "", -10) );
var_dump( strripos($haystack, "", -26) );
var_dump( strripos($haystack, "", -44) );
?>

DONE
--EXPECT--
int(44)
int(43)
int(34)
int(18)
int(0)

DONE
