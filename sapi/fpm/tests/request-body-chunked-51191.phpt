--TEST--
FPM: Test reading in request body without Content-Length header
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
var_dump(file_get_contents("php://input"));
echo "Test End\n";
EOT;

$headers = [];
$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request(
        '',
        [],
        null,
        null,
        null,
        null,
        false,
        "The body"
    )->expectBody(
        [
            'Test Start',
            'string(8) "The body"',
            'Test End',
        ]
    );
$tester->terminate();
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
