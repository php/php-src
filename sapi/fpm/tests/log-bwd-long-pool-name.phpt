--TEST--
FPM: Buffered worker output decorated log with pool name longer than the message prefix buffer
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$pool = str_repeat('p', 600);

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
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
/* The message prefix is truncated to 511 bytes. */
$tester->expectLogPattern('/^\[[^\]]+\] WARNING: \[pool p{505}"msg 1"$/');
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
