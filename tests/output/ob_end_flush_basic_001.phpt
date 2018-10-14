--TEST--
Test ob_end_flush() function : basic functionality
--FILE--
<?php
/* Prototype  : proto bool ob_end_flush(void)
 * Description: Flush (send) the output buffer, and delete current output buffer
 * Source code: main/output.c
 * Alias to functions:
 */

echo "*** Testing ob_end_flush() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing ob_end_flush() function with Zero arguments --\n";
var_dump(ob_end_flush());

ob_start();
var_dump(ob_end_flush());

ob_start();
echo "Hello\n";
var_dump(ob_end_flush());

var_dump(ob_end_flush());

echo "Done";
?>
--EXPECTF--
*** Testing ob_end_flush() : basic functionality ***

-- Testing ob_end_flush() function with Zero arguments --

Notice: ob_end_flush(): failed to delete and flush buffer. No buffer to delete or flush in %s on line 12
bool(false)
bool(true)
Hello
bool(true)

Notice: ob_end_flush(): failed to delete and flush buffer. No buffer to delete or flush in %s on line 21
bool(false)
Done
