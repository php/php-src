--TEST--
Test preg_match() function : error conditions - wrong numbers of parameters
--FILE--
<?php
/*
 *  proto int preg_match(string pattern, string subject [, array subpatterns [, int flags [, int offset]]])
 * Function is implemented in ext/pcre/php_pcre.c
*/
echo "*** Testing preg_match() : error conditions ***\n";
// Zero arguments
echo "\n-- Testing preg_match() function with Zero arguments --\n";
var_dump(preg_match());
//Test preg_match with one more than the expected number of arguments
echo "\n-- Testing preg_match() function with more than expected no. of arguments --\n";
$pattern = '/\w/';
$subject = 'string_val';
$flags = PREG_OFFSET_CAPTURE;
$offset = 10;
$extra_arg = 10;
var_dump(preg_match($pattern, $subject, $matches, $flags, $offset, $extra_arg));
// Testing preg_match withone less than the expected number of arguments
echo "\n-- Testing preg_match() function with less than expected no. of arguments --\n";
$pattern = '/\w/';
var_dump(preg_match($pattern));
echo "Done"
?>
--EXPECTF--
*** Testing preg_match() : error conditions ***

-- Testing preg_match() function with Zero arguments --

Warning: preg_match() expects at least 2 parameters, 0 given in %spreg_match_error.php on line %d
bool(false)

-- Testing preg_match() function with more than expected no. of arguments --

Warning: preg_match() expects at most 5 parameters, 6 given in %spreg_match_error.php on line %d
bool(false)

-- Testing preg_match() function with less than expected no. of arguments --

Warning: preg_match() expects at least 2 parameters, 1 given in %spreg_match_error.php on line %d
bool(false)
Done
