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

// No ob_start() executed
var_dump( ob_get_flush() );

?>
--EXPECTF--
*** Testing ob_get_flush() : error conditions ***

Notice: ob_get_flush(): failed to delete and flush buffer. No buffer to delete or flush in %s on line %d
bool(false)
