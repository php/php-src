--TEST--
Test ob_implicit_flush() function : check return value (always null).
--FILE--
<?php
/* Prototype  : proto void ob_implicit_flush([int flag])
 * Description: Turn implicit flush on/off and is equivalent to calling flush() after every output call
 * Source code: main/output.c
 * Alias to functions:
 */

echo "*** Testing ob_implicit_flush() : check return value ***\n";

var_dump(ob_implicit_flush());
var_dump(ob_implicit_flush(true));
var_dump(ob_implicit_flush(false));

echo "Done";
?>
--EXPECT--
*** Testing ob_implicit_flush() : check return value ***
NULL
NULL
NULL
Done
