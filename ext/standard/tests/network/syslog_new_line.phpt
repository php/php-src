--TEST--
Test syslog() function : new line in message
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
if (!defined('LOG_PERROR'))
  die("skip LOG_PERROR is not available on this platform");
?>
--FILE--
<?php
$priority = LOG_WARNING;
$message = "First line\nSecond line";

openlog('PHPT', LOG_PERROR, LOG_USER);
syslog($priority, $message);

?>
--EXPECTF--
%SPHPT%S%r(:|-)%r First line
%SPHPT%S%r(:|-)%r Second line
