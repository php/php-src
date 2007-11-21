--TEST--
Test preg_replace() function : error - incorrect number of parameters
--FILE--
<?php
/*
* proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, count]])
* Function is implemented in ext/pcre/php_pcre.c
*/
echo "*** Testing preg_replace() : error conditions ***\n";
//Zero arguments
echo "\n-- Testing preg_replace() function with zero arguments --\n";
var_dump(preg_replace());
//Test preg_replace() with one more than the expected number of arguments
echo "\n-- Testing preg_replace() function with more than expected no. of arguments --\n";
$regex = '/\w/';
$replace = '1';
$subject = 'string_val';
$limit = 10;
$extra_arg = 10;
var_dump(preg_replace($regex, $replace, $subject, $limit, $count, $extra_arg));
//Testing preg_replace() with one less than the expected number of arguments
echo "\n-- Testing preg_replace() function with less than expected no. of arguments --\n";
$regex = '/\w/';
$replace = '1';
var_dump(preg_replace($regex, $replace));
echo "Done"
?>
--EXPECTF--
*** Testing preg_replace() : error conditions ***

-- Testing preg_replace() function with zero arguments --

Warning: Wrong parameter count for preg_replace() in %spreg_replace_error.php on line %d
NULL

-- Testing preg_replace() function with more than expected no. of arguments --

Warning: Wrong parameter count for preg_replace() in %spreg_replace_error.php on line %d
NULL

-- Testing preg_replace() function with less than expected no. of arguments --

Warning: Wrong parameter count for preg_replace() in %spreg_replace_error.php on line %d
NULL
Done
