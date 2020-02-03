--TEST--
FPM: Function getallheaders basic test
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
var_dump(getallheaders());
echo "Test End\n";
EOT;

$headers = [];
$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request(
        '',
        [
            'HTTP_X_FOO' => 'BAR',
            'HTTP_FOO'   => 'foo'
        ]
    )->expectBody(
        [
            'Test Start',
            'array(4) {',
            '  ["Foo"]=>',
            '  string(3) "foo"',
            '  ["X-Foo"]=>',
            '  string(3) "BAR"',
            '  ["Content-Length"]=>',
            '  string(1) "0"',
            '  ["Content-Type"]=>',
            '  string(0) ""',
            '}',
            'Test End',
        ]
    );
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
