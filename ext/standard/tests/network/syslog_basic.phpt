--TEST--
Test syslog() function : basic functionality
--FILE--
<?php
/* Prototype  : bool syslog(int priority, string message)
 * Description: Generate a system log message
 * Source code: ext/standard/syslog.c
 * Alias to functions:
 */

echo "*** Testing syslog() : basic functionality ***\n";


// Initialise all required variables
$priority = LOG_WARNING;
$message = 'A test syslog call invocation';

// Calling syslog() with all possible arguments
var_dump( syslog($priority, $message) );

?>
--EXPECT--
*** Testing syslog() : basic functionality ***
bool(true)
