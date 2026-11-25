--TEST--
FPM: Log message with errno is truncated to the buffer when log_limit is above it
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

/* The error line nearly fills the 2048-byte log buffer before errno text is appended. */
$slowlog = '/' . str_repeat('a', 1970);

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_limit = 4096
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
request_slowlog_timeout = 1
slowlog = $slowlog
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogPattern('/^\[[^\]]+\] ERROR: Unable to create or open slowlog\(\/a{1970}\): .{7}\.\.\.$/');
$tester->close(true);

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
