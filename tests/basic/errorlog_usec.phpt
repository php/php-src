--TEST--
Microseconds in error log file
?>
--INI--
error_log=error_usec_test.log
error_log_usec=On
--FILE--
<?php

define('LOG_FILENAME', ini_get("error_log"));

if (file_exists(LOG_FILENAME)) {
    unlink(LOG_FILENAME);
}

error_log("hello world");

assert(file_exists(LOG_FILENAME));

printf("errorlog contents\n%s", file_get_contents(LOG_FILENAME));

?>
--CLEAN--
<?php
unlink("error_usec_test.log");
?>
--EXPECTF--
errorlog contents
[%d-%s-%d %d:%d:%d.%d %s] hello world
