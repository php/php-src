--TEST--
FPM: Test excluding URIs from access log
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

function doTestCalls(FPM\Tester &$tester, bool $expectSuppressableEntries)
{
    $tester->expectSuppressableAccessLogEntries($expectSuppressableEntries);

    $tester->ping();
    $tester->expectAccessLog("'GET /ping' 200", suppressable: true);

    $tester->request()->expectBody('OK');
    $tester->expectAccessLog("'GET /log-suppress-output.src.php' 200", suppressable: true);

    $tester->ping();
    $tester->expectAccessLog("'GET /ping' 200", suppressable: true);

    $tester->request()->expectBody('OK');
    $tester->expectAccessLog("'GET /log-suppress-output.src.php' 200", suppressable: true);

    $tester->ping();
    $tester->expectAccessLog("'GET /ping' 200", suppressable: true);

    $tester->request(query: 'test=output')->expectBody('output');
    $tester->expectAccessLog("'GET /log-suppress-output.src.php?test=output' 200", suppressable: false);

    $tester->ping();
    $tester->expectAccessLog("'GET /ping' 200", suppressable: true);

    $tester->request()->expectBody('OK');
    $tester->expectAccessLog("'GET /log-suppress-output.src.php' 200", suppressable: true);

    $tester->request(query: 'test=output', uri: '/ping')->expectBody('pong', 'text/plain');
    $tester->expectAccessLog("'GET /ping?test=output' 200", suppressable: false);

    $tester->request(headers: ['X_ERROR' => 1])->expectStatus('500 Internal Server Error')->expectBody('Not OK');
    $tester->expectAccessLog("'GET /log-suppress-output.src.php' 500", suppressable: false);

    $tester->request()->expectBody('OK');
    $tester->expectAccessLog("'GET /log-suppress-output.src.php' 200", suppressable: true);

    $tester->request(query: 'test=output', uri: '/ping')->expectBody('pong', 'text/plain');
    $tester->expectAccessLog("'GET /ping?test=output' 200", suppressable: false);

    $tester->ping();
    $tester->expectAccessLog("'GET /ping' 200", suppressable: true);
}

$src = <<<EOT
<?php
if (isset(\$_SERVER['X_ERROR'])) {
    http_response_code(500);
    echo "Not OK";
    exit;
}
echo \$_REQUEST['test'] ?? "OK";
EOT;

$cfg = <<<EOT
[global]
error_log = {{RFILE:LOG:ERR}}
pid = {{RFILE:PID}}
[unconfined]
listen = {{ADDR}}
access.log = {{RFILE:LOG:ACC}}
access.format = "'%m %r%Q%q' %s"
slowlog = {{RFILE:LOG:SLOW}}
request_slowlog_timeout = 1
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$prefix = __DIR__;
$tester = new FPM\Tester($cfg, $src);
$tester->start(['--prefix', $prefix]);
$tester->expectLogStartNotices();
doTestCalls($tester, expectSuppressableEntries: true);
// Add source file and ping to ignore list
$cfg = <<<EOT
[global]
error_log = {{RFILE:LOG:ERR}}
pid = {{RFILE:PID}}
[unconfined]
listen = {{ADDR}}
access.log = {{RFILE:LOG:ACC}}
access.format = "'%m %r%Q%q' %s"
access.suppress_path[] = /ping
access.suppress_path[] = /log-suppress-output.src.php
slowlog = {{RFILE:LOG:SLOW}}
request_slowlog_timeout = 1
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;
$tester->reload($cfg);
$tester->expectLogReloadingNotices();
doTestCalls($tester, expectSuppressableEntries: false);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
$tester->expectNoFile(FPM\Tester::FILE_EXT_PID, $prefix);
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
