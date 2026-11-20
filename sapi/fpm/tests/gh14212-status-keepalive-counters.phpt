--TEST--
FPM: GH-14212 (Status counters remain accurate over fresh and persistent FastCGI connections)
--EXTENSIONS--
sockets
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[fresh]
listen = {{ADDR[fresh]}}
pm = static
pm.max_children = 1
pm.status_path = /status
[reused]
listen = {{ADDR[reused]}}
pm = static
pm.max_children = 1
pm.status_path = /status
EOT;

$tester = new FPM\Tester($cfg);
$tester->start();
$tester->expectLogStartNotices();

try {
    foreach (['fresh' => false, 'reused' => true] as $pool => $keepAlive) {
        $pid = null;
        for ($i = 1; $i <= 5; $i++) {
            $response = $tester->request(
                address: '{{ADDR[' . $pool . ']}}',
                query: 'json&full',
                uri: '/status',
                connKeepAlive: $keepAlive,
                socketKeepAlive: $keepAlive,
            );
            $status = json_decode($response->getBody('application/json'), true, flags: JSON_THROW_ON_ERROR);
            if (count($status['processes']) !== 1) {
                throw new Exception('Expected one actual worker');
            }
            $currentPid = $status['processes'][0]['pid'];
            if ($pid !== null && $currentPid !== $pid) {
                throw new Exception('Worker changed during the test');
            }
            $pid = $currentPid;
            foreach (['active processes', 'total processes', 'max active processes'] as $field) {
                if ($status[$field] !== 1) {
                    throw new Exception("Request $i: $field is {$status[$field]}, expected 1");
                }
            }
            if ($status['idle processes'] !== 0) {
                throw new Exception('Expected no idle workers while serving the status request');
            }
            if ($status['accepted conn'] !== $i) {
                throw new Exception("Request $i: accepted conn is {$status['accepted conn']}, expected $i");
            }
        }
    }
} finally {
    $tester->terminate();
    $tester->expectLogTerminatingNotices();
    $tester->close();
}

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
