--TEST--
FPM: bug64539 - status json format escaping
--SKIPIF--
<?php
include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 2
pm.status_path = /status
catch_workers_output = yes
EOT;

$code = <<<EOT
<?php
usleep(200000);
EOT;

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$responses = $tester
    ->multiRequest([
        ['query' => 'a=b"c'],
        ['uri' => '/status', 'query' => 'full&json', 'delay' => 100000],
    ]);
$responses[1]->expectJsonBodyPatternForStatusProcessField('request uri', '\?a=b"c$');
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
