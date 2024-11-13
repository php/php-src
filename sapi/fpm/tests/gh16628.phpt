--TEST--
GH-16628 (FPM logs are getting corrupted with this log statement)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_level = debug
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

$code = <<<'EOT'
<?php
for ($i = 1; $i < 100; $i++) {
	zend_test_log_err_debug(str_repeat("a", $i));
}
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(extensions: ['zend_test']);
$tester->expectLogStartNotices();
$tester->request()->expectEmptyBody();
for ($i = 1; $i < 100; $i++) {
    $tester->expectLogNotice("%sPHP message: " . str_repeat("a", $i));
}
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
