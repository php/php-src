--TEST--
Test syslog() function : new line in message
--FILE--
<?php
$priority = LOG_WARNING;
$message = "First line\nSecond line";

openlog('PHPT', LOG_PERROR, LOG_USER);
syslog($priority, $message);

?>
--EXPECT--
PHPT: First line
PHPT: Second line
