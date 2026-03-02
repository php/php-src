--TEST--
FPM: Log message in shutdown function
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_limit = 1024
log_buffering = yes
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
register_shutdown_function(function() {
    error_log(str_repeat('e', 80));
});
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
$tester->terminate();
$tester->expectFastCGIErrorMessage('e', 1050, 80);
$tester->expectLogMessage('NOTICE: PHP message: ' . str_repeat('e', 80), 1050);
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
