--TEST--
FPM: GH-8885 - access.log with stderr begins to write logs to error_log after daemon reload
--SKIPIF--
<?php
include "skipif.inc";
FPM\Tester::skipIfRoot();
?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
pid = {{FILE:PID}}
[unconfined]
listen = {{ADDR}}
access.log=/dev/stderr
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
php_flag[expose_php] = on
EOT;

// php-fpm must not be launched with --force-stderr option
$tester = new FPM\Tester($cfg, '', [FPM\Tester::PHP_FPM_DISABLE_FORCE_STDERR => true]);
// getPrefixedFile('err.log') is the same path that returns processTemplate('{{FILE:LOG}}')
$errorLogFile = $tester->getPrefixedFile('err.log');

$tester->start();
$tester->expectNoLogMessages();

$content = file_get_contents($errorLogFile);
assert($content !== false && strlen($content) > 0, 'File must not be empty');

$errorLogLines = explode("\n", $content);
array_pop($errorLogLines);

assert(count($errorLogLines) === 2, 'Expected 2 records in the error_log file');
assert(strpos($errorLogLines[0], 'NOTICE: fpm is running, pid'));
assert(strpos($errorLogLines[1], 'NOTICE: ready to handle connections'));

$tester->ping('{{ADDR}}');
$stderrLines = $tester->getLogLines(-1);
assert(count($stderrLines) === 1, 'Expected 1 record in the stderr output (access.log)');
$stderrLine = $stderrLines[0];
assert(preg_match('/127.0.0.1 .* "GET \/ping" 200$/', $stderrLine), 'Incorrect format of access.log record');

$tester->signal('USR2');
$tester->expectLogNotice('using inherited socket fd=\d+, "127.0.0.1:\d+"');

$content = file_get_contents($errorLogFile);
assert($content !== false && strlen($content) > 0, 'File must not be empty');
$errorLogLines = explode("\n", $content);
array_pop($errorLogLines);

assert(count($errorLogLines) >= 5, 'Expected at least 5 records in the error_log file');
assert(strpos($errorLogLines[0], 'NOTICE: fpm is running, pid'));
assert(strpos($errorLogLines[1], 'NOTICE: ready to handle connections'));
assert(strpos($errorLogLines[2], 'NOTICE: Reloading in progress'));
assert(strpos($errorLogLines[3], 'NOTICE: reloading: execvp'));
assert(strpos($errorLogLines[4], 'NOTICE: using inherited socket'));

$tester->ping('{{ADDR}}');
$stderrLines = $tester->getLogLines(-1);
assert(count($stderrLines) === 1, 'Must be only 1 record in the access.log');
assert(preg_match('/127.0.0.1 .* "GET \/ping" 200$/', $stderrLines[0]), 'Incorrect format of access.log record');

$tester->terminate();
$stderrLines = $tester->expectNoLogMessages();

$content = file_get_contents($errorLogFile);
assert($content !== false && strlen($content) > 0, 'File must not be empty');
$errorLogLines = explode("\n", $content);
array_pop($errorLogLines);
$errorLogLastLine = array_pop($errorLogLines);
assert(strpos($errorLogLastLine, 'NOTICE: exiting, bye-bye'));

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
