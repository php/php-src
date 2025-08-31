--TEST--
GH-8646 (Memory leak PHP FPM 8.1)
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
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$code = <<<EOT
<?php
class MyClass {}
echo zend_get_map_ptr_last();
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$map_ptr_last_values = [];
for ($i = 0; $i < 10; $i++) {
    $map_ptr_last_values[] = (int) $tester->request()->getBody();
}
// Ensure that map_ptr_last did not increase
var_dump(count(array_unique($map_ptr_last_values, SORT_REGULAR)) === 1);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
bool(true)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
