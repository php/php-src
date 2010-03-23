--TEST--
Test ob_get_flush() function : error conditions 
--INI--
output_buffering=0
--FILE--
<?php
/* Prototype  : bool ob_get_flush(void)
 * Description: Get current buffer contents, flush (send) the output buffer, and delete current output buffer 
 * Source code: main/output.c
 * Alias to functions: 
 */

echo "*** Testing ob_get_flush() : error conditions ***\n";

// One extra argument
$extra_arg = 10;
var_dump( ob_get_flush( $extra_arg ) );

// No ob_start() executed
var_dump( ob_get_flush() );

?>
===DONE===
--EXPECTF--
*** Testing ob_get_flush() : error conditions ***

Warning: ob_get_flush() expects exactly 0 parameters, 1 given in %s on line %d
NULL
bool(false)
===DONE===
