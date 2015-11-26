--TEST--
Test ob_end_flush() function : error conditions 
--FILE--
<?php
/* Prototype  : proto bool ob_end_flush(void)
 * Description: Flush (send) the output buffer, and delete current output buffer 
 * Source code: main/output.c
 * Alias to functions: 
 */

echo "*** Testing ob_end_flush() : error conditions ***\n";

// One argument
echo "\n-- Testing ob_end_flush() function with one argument --\n";
$extra_arg = 10;;
var_dump( ob_end_flush($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing ob_end_flush() : error conditions ***

-- Testing ob_end_flush() function with one argument --

Warning: ob_end_flush() expects exactly 0 parameters, 1 given in %s on line 13
NULL
Done
