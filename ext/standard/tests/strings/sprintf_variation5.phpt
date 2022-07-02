--TEST--
sprintf With signed integer
--FILE--
<?php

/* example#5: various examples */
$n =  43951789;
$u = -43951789;
$c = 65; // ASCII 65 is 'A'

// notice the double %%, this prints a literal '%' character
var_dump(sprintf("%%b = '%b'", $n)); // binary representation
var_dump(sprintf("%%c = '%c'", $c)); // print the ascii character, same as chr() function
var_dump(sprintf("%%d = '%d'", $n)); // standard integer representation
var_dump(sprintf("%%e = '%e'", $n)); // scientific notation
var_dump(sprintf("%%f = '%f'", $n)); // floating point representation
var_dump(sprintf("%%o = '%o'", $n)); // octal representation
var_dump(sprintf("%%s = '%s'", $n)); // string representation
var_dump(sprintf("%%x = '%x'", $n)); // hexadecimal representation (lower-case)
var_dump(sprintf("%%X = '%X'", $n)); // hexadecimal representation (upper-case)

var_dump(sprintf("%%+d = '%+d'", $n)); // sign specifier on a positive integer
var_dump(sprintf("%%+d = '%+d'", $u)); // sign specifier on a negative integer

?>
--EXPECT--
string(33) "%b = '10100111101010011010101101'"
string(8) "%c = 'A'"
string(15) "%d = '43951789'"
string(18) "%e = '4.395179e+7'"
string(22) "%f = '43951789.000000'"
string(16) "%o = '247523255'"
string(15) "%s = '43951789'"
string(14) "%x = '29ea6ad'"
string(14) "%X = '29EA6AD'"
string(17) "%+d = '+43951789'"
string(17) "%+d = '-43951789'"
