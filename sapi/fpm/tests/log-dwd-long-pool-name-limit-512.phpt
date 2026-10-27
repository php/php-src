--TEST--
FPM: Direct worker output decorated log with pool name longer than log_limit
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$pool = str_repeat('p', 1100);

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_limit = 512
log_buffering = no
[$pool]
listen = {{ADDR}}
pm = static
pm.max_children = 1
catch_workers_output = yes
EOT;

$code = <<<EOT
<?php
file_put_contents('php://stderr', "msg 1\n");
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
/* The message prefix is truncated to half of log_limit. */
$tester->expectLogPattern('/^\[[^\]]+\] WARNING: \[pool p{250}"msg 1"$/');
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
