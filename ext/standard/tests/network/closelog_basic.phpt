--TEST--
Test closelog() function : basic functionality
--FILE--
<?php
/* Prototype  : bool closelog(void)
 * Description: Close connection to system logger
 * Source code: ext/standard/syslog.c
 * Alias to functions:
 */

echo "*** Testing closelog() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing closelog() function with Zero arguments --\n";
var_dump( closelog() );
?>
===DONE===
--EXPECT--
*** Testing closelog() : basic functionality ***

-- Testing closelog() function with Zero arguments --
bool(true)
===DONE===
