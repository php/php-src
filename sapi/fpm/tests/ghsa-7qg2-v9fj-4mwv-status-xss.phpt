--TEST--
FPM: GHSA-7qg2-v9fj-4mwv - status xss
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
                ['uri' => '/<script>alert(1)</script>', 'query' => '<script>alert(2)</script>'],
                ['uri' => '/status', 'query' => 'full&html', 'delay' => 100000],
        ]);
var_dump(strpos($responses[1]->getBody(), '<script>'));
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
bool(false)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
