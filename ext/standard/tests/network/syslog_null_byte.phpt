--TEST--
Test syslog() function : nul byte in message
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php
$priority = LOG_WARNING;
$message = "A simple \0 message";

openlog('PHPT', LOG_PERROR, LOG_USER);
syslog($priority, $message);

?>
--EXPECTF--
%SPHPT%S%r(:|-)%r A simple \x00 message
