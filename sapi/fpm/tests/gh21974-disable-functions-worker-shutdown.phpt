--TEST--
FPM: GH-21974 - disable_functions set in a pool must not break shutdown of the worker
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
pm = static
pm.max_children = 1
; The worker shuts down gracefully after the first request.
pm.max_requests = 1
php_admin_value[disable_functions] = system
EOT;

$code = <<<EOT
<?php
echo function_exists('system') ? 'enabled' : 'disabled';
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectBody('disabled');
$tester->expectLogNotice('child \d+ exited with code 0 after \d+\.\d+ seconds from start', 'unconfined');
$tester->expectLogNotice('child \d+ started', 'unconfined');
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
