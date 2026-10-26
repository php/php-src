--TEST--
FPM: GHSA-3mg3-cr28-46gc - status xss via request method, auth user and script filename
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
pm.max_children = 3
pm.status_path = /status
catch_workers_output = yes
EOT;

$code = <<<EOT
<?php
usleep(300000);
EOT;

$scriptFilename = __DIR__ . '/ghsa-3mg3-cr28-46gc-<svg>.php';
file_put_contents($scriptFilename, $code);

$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$responses = $tester
        ->multiRequest([
                [
                    'uri' => '/ghsa-3mg3-cr28-46gc-<svg>.php',
                    'headers' => [
                        'REQUEST_METHOD' => '<svg>',
                        'HTTP_AUTHORIZATION' => 'Basic ' . base64_encode('"<svg onload=x>:x'),
                        'SCRIPT_FILENAME' => $scriptFilename,
                    ],
                ],
                ['uri' => '/status', 'query' => 'full&html', 'delay' => 100000],
                ['uri' => '/status', 'query' => 'full&json', 'delay' => 100000],
        ]);
var_dump(strpos($responses[1]->getBody(), '<svg'));
var_dump(substr_count($responses[1]->getBody(), '&lt;svg'));
$status = json_decode($responses[2]->getBody('application/json'), true);
foreach ($status['processes'] as $proc) {
    if ($proc['request method'] === '<svg>') {
        var_dump($proc['user'], $proc['script'] === $scriptFilename);
    }
}
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
bool(false)
int(4)
string(15) ""<svg onload=x>"
bool(true)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
@unlink(__DIR__ . '/ghsa-3mg3-cr28-46gc-<svg>.php');
?>
