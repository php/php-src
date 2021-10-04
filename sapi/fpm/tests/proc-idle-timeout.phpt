--TEST--
FPM: Process manager config pm.process_idle_timeout
--SKIPIF--
<?php
include "skipif.inc";
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
echo "skip Test fails in CI\n";
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = ondemand
pm.max_children = 3
pm.process_idle_timeout = 1
pm.status_path = /status
EOT;

$code = <<<EOT
<?php
usleep(200);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->multiRequest(2);
$tester->status([
    'total processes' => 2,
]);
// wait for process idle timeout
sleep(6);
$tester->status([
    'total processes' => 1,
]);
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
