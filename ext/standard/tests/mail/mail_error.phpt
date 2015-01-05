--TEST--
Test mail() function : error conditions 
--FILE--
<?php
/* Prototype  : int mail(string to, string subject, string message [, string additional_headers [, string additional_parameters]])
 * Description: Send an email message 
 * Source code: ext/standard/mail.c
 * Alias to functions: 
 */

echo "*** Testing mail() : error conditions ***\n";


//Test mail with one more than the expected number of arguments
echo "\n-- Testing mail() function with more than expected no. of arguments --\n";
$to = 'string_val';
$subject = 'string_val';
$message = 'string_val';
$additional_headers = 'string_val';
$additional_parameters = 'string_val';
$extra_arg = 10;
var_dump( mail($to, $subject, $message, $additional_headers, $additional_parameters, $extra_arg) );

// Testing mail with one less than the expected number of arguments
echo "\n-- Testing mail() function with less than expected no. of arguments --\n";
$to = 'string_val';
$subject = 'string_val';
var_dump( mail($to, $subject) );

?>
===DONE===
--EXPECTF--
*** Testing mail() : error conditions ***

-- Testing mail() function with more than expected no. of arguments --

Warning: mail() expects at most 5 parameters, 6 given in %s on line %d
NULL

-- Testing mail() function with less than expected no. of arguments --

Warning: mail() expects at least 3 parameters, 2 given in %s on line %d
NULL
===DONE===
