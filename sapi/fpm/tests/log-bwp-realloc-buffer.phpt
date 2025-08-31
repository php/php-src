--TEST--
FPM: bug81513 - Buffered worker output plain log stream reallocation
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
decorate_workers_output = no
EOT;

$code = <<<EOT
<?php
file_put_contents('php://stderr', str_repeat('a', 100));
usleep(20000);
file_put_contents('php://stderr', str_repeat('b', 2500) . "\n");
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
$tester->terminate();
$tester->expectLogLine(str_repeat('a', 100)  . str_repeat('b', 923), decorated: false);
$tester->expectLogLine(str_repeat('b', 1023), decorated: false);
$tester->expectLogLine(str_repeat('b', 554), decorated: false);
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
