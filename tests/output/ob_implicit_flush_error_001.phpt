--TEST--
Test ob_implicit_flush() function : wrong number of arguments
--FILE--
<?php
/* Prototype  : proto void ob_implicit_flush([int flag])
 * Description: Turn implicit flush on/off and is equivalent to calling flush() after every output call
 * Source code: main/output.c
 * Alias to functions:
 */

echo "*** Testing ob_implicit_flush() : error conditions ***\n";


//Test ob_implicit_flush with one more than the expected number of arguments
echo "\n-- Testing ob_implicit_flush() function with more than expected no. of arguments --\n";
$flag = 10;
$extra_arg = 10;
var_dump( ob_implicit_flush($flag, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing ob_implicit_flush() : error conditions ***

-- Testing ob_implicit_flush() function with more than expected no. of arguments --

Warning: ob_implicit_flush() expects at most 1 parameter, 2 given in %s on line 15
NULL
Done
