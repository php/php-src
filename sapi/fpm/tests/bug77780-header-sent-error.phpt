--TEST--
FPM: bug77780 - Headers already sent error incorrectly emitted
--SKIPIF--
<?php include "skipif.inc"; ?>
--EXTENSIONS--
session
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
EOT;

$code = <<<EOT
<?php
echo str_repeat('asdfghjkl', 150000) . "\n";
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start(extensions: ['session']);
$tester->expectLogStartNotices();
$tester
    ->request(
        headers: [
            'PHP_VALUE' => "session.cookie_secure=1",
        ],
        readLimit: 10,
        expectError: true
    );
$tester->request(
        headers: [
            'PHP_VALUE' => "session.cookie_secure=1",
        ]
    )
    ->expectNoError();
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
