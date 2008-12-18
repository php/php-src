--TEST--
Test ob_get_level() function : error conditions 
--FILE--
<?php
/* Prototype  : proto int ob_get_level(void)
 * Description: Return the nesting level of the output buffer 
 * Source code: main/output.c
 * Alias to functions: 
 */

echo "*** Testing ob_get_level() : error conditions ***\n";

// One argument
echo "\n-- Testing ob_get_level() function with one argument --\n";
$extra_arg = 10;;
var_dump( ob_get_level($extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing ob_get_level() : error conditions ***

-- Testing ob_get_level() function with one argument --

Warning: Wrong parameter count for ob_get_level() in %s on line 13
NULL
Done