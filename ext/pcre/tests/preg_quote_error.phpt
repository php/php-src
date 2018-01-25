--TEST--
Test preg_quote() function : error conditions  - wrong numbers of parameters
--FILE--
<?php
/*
* proto string preg_quote(string str [, string delim_char])
* Function is implemented in ext/pcre/php_pcre.c
*/
echo "*** Testing preg_quote() : error conditions ***\n";
// Zero arguments
echo "\n-- Testing preg_quote() function with Zero arguments --\n";
var_dump(preg_quote());
//Test preg_quote with one more than the expected number of arguments
echo "\n-- Testing preg_quote() function with more than expected no. of arguments --\n";
$str = 'string_val';
$delim_char = '/';
$extra_arg = 10;
var_dump(preg_quote($str, $delim_char, $extra_arg));
echo "Done"
?>
--EXPECTF--
*** Testing preg_quote() : error conditions ***

-- Testing preg_quote() function with Zero arguments --

Warning: preg_quote() expects at least 1 parameter, 0 given in %spreg_quote_error.php on line %d
NULL

-- Testing preg_quote() function with more than expected no. of arguments --

Warning: preg_quote() expects at most 2 parameters, 3 given in %spreg_quote_error.php on line %d
NULL
Done
