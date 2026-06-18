--TEST--
FPM: test HOST= ini sections
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

$ini = <<<EOT
memory_limit = 100M
[HOST=foo.bar]
memory_limit = 200M
[HOST=bar.foo]
memory_limit = 300M
EOT;

$code = <<<EOT
<?php
echo "Test Start\n";
var_dump(ini_get('memory_limit'));
echo "Test End\n";
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: $ini);
$tester->expectLogStartNotices();
$tester->request()->expectBody([
    'Test Start',
    'string(4) "100M"',
    'Test End'
]);
$tester->request(headers: [ "SERVER_NAME" => "foo.bar" ])->expectBody([
    'Test Start',
    'string(4) "200M"',
    'Test End'
]);
$tester->request(headers: [ "SERVER_NAME" => "bar.foo" ])->expectBody([
    'Test Start',
    'string(4) "300M"',
    'Test End'
]);
# check if the ini value has been reset
$tester->request()->expectBody([
    'Test Start',
    'string(4) "100M"',
    'Test End'
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
