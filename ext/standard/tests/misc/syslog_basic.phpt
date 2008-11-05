--TEST--
Basic syslog test
--FILE--
<?php
openlog('phpt', LOG_NDELAY | LOG_PID, LOG_USER);

syslog(LOG_WARNING, 'Basic syslog test');

closelog();
?>
===DONE===
--EXPECT--
===DONE===
