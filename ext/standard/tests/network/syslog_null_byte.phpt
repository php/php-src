--TEST--
Test syslog() function : nul byte in message
--FILE--
<?php
$priority = LOG_WARNING;
$message = "A simple \0 message";

openlog('PHPT', LOG_PERROR, LOG_USER);
syslog($priority, $message);

?>
--EXPECT--
PHPT: A simple \x00 message
