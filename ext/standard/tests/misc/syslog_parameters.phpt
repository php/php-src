--TEST--
Syslog parameter parsing test
--FILE--
<?php
openlog();
openlog(NULL, 'string', 0);

syslog();
syslog('Wrong parameter order', LOG_WARNING);

closelog('Doesnt take any parameters');
?>
--EXPECTF--
Warning: openlog() expects exactly 3 parameters, 0 given in %s on line %d

Warning: openlog() expects parameter 2 to be integer, string given in %s on line %d

Warning: syslog() expects exactly 2 parameters, 0 given in %s on line %d

Warning: syslog() expects parameter 1 to be integer, string given in %s on line %d

Warning: closelog() expects exactly 0 parameters, 1 given in %s on line %d
