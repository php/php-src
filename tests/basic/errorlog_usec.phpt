--TEST--
Microseconds in error log file
--INI--
error_log=error_usec_test.log
error_log_usec=On
--FILE--
<?php

error_log("hello world");

printf("errorlog contents\n%s", file_get_contents(ini_get("error_log")));

?>
--CLEAN--
<?php
unlink("error_usec_test.log");
?>
--EXPECTF--
errorlog contents
[%d-%s-%d %d:%d:%d.%d %s] hello world
