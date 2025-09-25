--TEST--
FPM: test ini settings from fcgi env
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
echo "Test Start\n";
var_dump(ini_get('memory_limit'));
echo "Test End\n";
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [ 'memory_limit' => '100M' ]);
$tester->expectLogStartNotices();
$tester->request()->expectBody([
    'Test Start',
    'string(4) "100M"',
    'Test End'
]);
$tester->request(headers: [ "PHP_VALUE" => "memory_limit=300M" ])->expectBody([
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
$tester->request(headers: [ "PHP_VALUE" => "memory_limit=" ])->expectBody([
    'Test Start',
    'string(4) "100M"',
    'Test End'
])->expectError('Passing INI directive through FastCGI: unable to set \'memory_limit\'');
$tester->request(headers: [ "PHP_VALUE" => "memory_limit" ])->expectBody([
    'Test Start',
    'string(4) "100M"',
    'Test End'
])->expectError('Passing INI directive through FastCGI: empty value for key \'memory_limit\'');
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
