--TEST--
FPM: status encodes the request method, user and script of the processes
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
foreach (['html' => 'text/html', 'xml' => 'text/xml', 'json' => 'application/json'] as $format => $type) {
    $responses = $tester
        ->multiRequest([
            ['headers' => [
                'REQUEST_METHOD' => 'GET<script>alert(1)</script>',
                'HTTP_AUTHORIZATION' => 'Basic ' . base64_encode('<script>alert(2)</script>":pass'),
            ]],
            ['uri' => '/status', 'query' => "full&$format", 'delay' => 100000],
        ]);
    $body = $responses[1]->getBody($type);
    echo $format, ": ";
    if ($format === 'json') {
        $processes = json_decode($body, true)['processes'];
        var_dump(in_array('GET<script>aler', array_column($processes, 'request method'), true));
        var_dump(in_array('<script>alert(2)</script>"', array_column($processes, 'user'), true));
    } else {
        var_dump(strpos($body, '<script>alert'));
    }
}
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECT--
html: bool(false)
xml: bool(false)
json: bool(true)
bool(true)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
