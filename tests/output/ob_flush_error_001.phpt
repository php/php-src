--TEST--
Test ob_flush() function : error conditions 
--FILE--
<?php
/* Prototype  : proto bool ob_flush(void)
 * Description: Flush (send) contents of the output buffer. The last buffer content is sent to next buffer 
 * Source code: main/output.c
 * Alias to functions: 
 */

echo "*** Testing ob_flush() : error conditions ***\n";

// One argument
echo "\n-- Testing ob_flush() function with one argument --\n";
$extra_arg = 10;;
var_dump( ob_flush($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing ob_flush() : error conditions ***

-- Testing ob_flush() function with one argument --

Warning: ob_flush() expects exactly 0 parameters, 1 given in %s on line 13
NULL
Done
