--TEST--
Test preg_split() function : error conditions - incorrect number of parameters
--FILE--
<?php
/*
* proto array preg_split(string pattern, string subject [, int limit [, int flags]])
* Function is implemented in ext/pcre/php_pcre.c
*/
echo "*** Testing preg_split() : error conditions ***\n";
// Zero arguments
echo "\n-- Testing preg_split() function with Zero arguments --\n";
var_dump(preg_split());
//Test preg_split with one more than the expected number of arguments
echo "\n-- Testing preg_split() function with more than expected no. of arguments --\n";
$pattern = '/_/';
$subject = 'string_val';
$limit = 10;
$flags = PREG_SPLIT_NO_EMPTY;
$extra_arg = 10;
var_dump(preg_split($pattern, $subject, $limit, $flags, $extra_arg));
// Testing preg_split withone less than the expected number of arguments
echo "\n-- Testing preg_split() function with less than expected no. of arguments --\n";
$pattern = '/\./';
var_dump(preg_split($pattern));
echo "Done"
?>
--EXPECTF--
*** Testing preg_split() : error conditions ***

-- Testing preg_split() function with Zero arguments --

Warning: preg_split() expects at least 2 parameters, 0 given in %spreg_split_error.php on line %d
bool(false)

-- Testing preg_split() function with more than expected no. of arguments --

Warning: preg_split() expects at most 4 parameters, 5 given in %spreg_split_error.php on line %d
bool(false)

-- Testing preg_split() function with less than expected no. of arguments --

Warning: preg_split() expects at least 2 parameters, 1 given in %spreg_split_error.php on line %d
bool(false)
Done