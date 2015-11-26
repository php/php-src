--TEST--
Test preg_match_all() function : error conditions - incorrect number of parameters
--FILE--
<?php
/*
* proto int preg_match_all(string pattern, string subject, array subpatterns [, int flags [, int offset]])
* Function is implemented in ext/pcre/php_pcre.c
*/
echo "*** Testing preg_match_all() : error conditions ***\n";
// Zero arguments
echo "\n-- Testing preg_match_all() function with Zero arguments --\n";
var_dump(preg_match_all());
//Test preg_match_all with one more than the expected number of arguments
echo "\n-- Testing preg_match_all() function with more than expected no. of arguments --\n";
$pattern = '/\w/';
$subject = 'string_val';
$flags = PREG_OFFSET_CAPTURE;
$offset = 10;
$extra_arg = 10;
var_dump(preg_match_all($pattern, $subject, $matches, $flags, $offset, $extra_arg));
// Testing preg_match_all withone less than the expected number of arguments
echo "\n-- Testing preg_match_all() function with less than expected no. of arguments --\n";
$pattern = '/\w/';
var_dump(preg_match_all($pattern));
echo "Done"
?>
--EXPECTF--
*** Testing preg_match_all() : error conditions ***

-- Testing preg_match_all() function with Zero arguments --

Warning: preg_match_all() expects at least 2 parameters, 0 given in %spreg_match_all_error.php on line %d
bool(false)

-- Testing preg_match_all() function with more than expected no. of arguments --

Warning: preg_match_all() expects at most 5 parameters, 6 given in %spreg_match_all_error.php on line %d
bool(false)

-- Testing preg_match_all() function with less than expected no. of arguments --

Warning: preg_match_all() expects at least 2 parameters, 1 given in %spreg_match_all_error.php on line %d
bool(false)
Done
