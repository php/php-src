--TEST--
GH-22616: Extension child initialization runs before FPM requests
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
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 2
EOT;

$code = <<<'EOT'
<?php
echo zend_test_child_init_count();
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(extensions: ['zend_test']);
$tester->expectLogStartNotices();
for ($i = 0; $i < 5; $i++) {
    $tester->request()->expectBody('1');
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
