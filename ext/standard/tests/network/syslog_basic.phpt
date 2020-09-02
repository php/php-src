--TEST--
Test syslog() function : basic functionality
--FILE--
<?php
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
