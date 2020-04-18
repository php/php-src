--TEST--
FPM: Buffered worker output decorated log with msg containing new lines
--SKIPIF--
<?php include "skipif.inc"; ?>
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
file_put_contents('php://stderr', "msg 1\nmsg 2\nmsg 3");
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
$tester->expectLogLine('msg 1');
$tester->expectLogLine('msg 2');
$tester->expectLogLine('msg 3');
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
