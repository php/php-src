--TEST--
FPM: Test URIs are not excluded from access log when there is a request body
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$testScript = <<<EOT
<?php
echo strlen(file_get_contents('php://input'));
EOT;

$body = str_repeat('a', 100);

// Add health checks to ignore list
$cfg = <<<EOT
[global]
error_log = {{FILE:LOG:ERR}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
access.log = {{FILE:LOG:ACC}}
access.format = "'%m %r%Q%q' %s"
access.suppress_path[] = /ping
access.suppress_path[] = /request-1
access.suppress_path[] = /request-2
access.suppress_path[] = /request-3
access.suppress_path[] = /request-4
access.suppress_path[] = /request-5
access.suppress_path[] = /request-6
slowlog = {{FILE:LOG:SLOW}}
request_slowlog_timeout = 1
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$tester = new FPM\Tester($cfg, $testScript);
$tester->start();
$tester->expectLogStartNotices();
$tester->expectSuppressableAccessLogEntries(false);
$tester->ping();

// Should not suppress POST with no body
$tester->request(
    uri: '/request-1',
    headers: ['REQUEST_METHOD' => 'POST']
)->expectBody('0');
$tester->expectAccessLog("'POST /request-1' 200", suppressable: false);

// Should not suppress POST with body
$tester->request(
    uri: '/request-2',
    stdin: $body
)->expectBody('100');
$tester->expectAccessLog("'POST /request-2' 200", suppressable: false);

// Should not suppress GET with body
$tester->request(
    uri: '/request-3',
    headers: ['REQUEST_METHOD' => 'GET'],
    stdin: $body
)->expectBody('100');
$tester->expectAccessLog("'GET /request-3' 200", suppressable: false);

// Should suppress GET with no body
$tester->request(
    uri: '/request-4'
)->expectBody('0');
$tester->expectAccessLog("'GET /request-4' 200", suppressable: true);

// Should not suppress GET with no body but incorrect content length
$tester->request(
    uri: '/request-5',
    headers: ['REQUEST_METHOD' => 'GET', 'CONTENT_LENGTH' => 100]
)->expectBody('0');
$tester->expectAccessLog("'GET /request-5' 200", suppressable: false);

// Should suppress GET with body but 0 content length (no stdin readable)
$tester->request(
    uri: '/request-6',
    headers: ['REQUEST_METHOD' => 'GET', 'CONTENT_LENGTH' => 0],
    stdin: $body
)->expectBody('0');
$tester->expectAccessLog("'GET /request-6' 200", suppressable: true);

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
$tester->expectNoFile(FPM\Tester::FILE_EXT_PID);
$tester->checkAccessLog();

?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
