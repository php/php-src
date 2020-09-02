--TEST--
FPM: Buffered worker output decorated log with multiple continuous messages (stdout/stderr mixed)
--SKIPIF--
<?php include "skipif.inc"; ?>
--XFAIL--
Fails intermittently
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
catch_workers_output = yes
EOT;

$code = <<<EOT
<?php
file_put_contents('php://stdout', "msg 1 - ");
usleep(1);
file_put_contents('php://stderr', "msg 2 - ");
usleep(1);
file_put_contents('php://stderr', "msg 3");
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
$tester->request()->expectEmptyBody();
$tester->expectLogLine('msg 1 - ', false);
$tester->expectLogLine('msg 2 - msg 3', true);
$tester->terminate();
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
