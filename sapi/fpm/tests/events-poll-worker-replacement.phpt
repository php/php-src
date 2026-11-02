--TEST--
FPM: poll continues capturing worker output after worker replacement
--SKIPIF--
<?php
include "skipif.inc";
if (PHP_OS_FAMILY !== 'Linux') {
    die('skip requires Linux poll EOF semantics for slot reuse (see php/php-src#25154)');
}
?>
--FILE--
<?php
require_once "tester.inc";

$cfg = <<<'EOT'
[global]
error_log = {{FILE:LOG}}
events.mechanism = poll
[first]
listen = {{ADDR[first]}}
pm = static
pm.max_children = 1
catch_workers_output = yes
decorate_workers_output = no
[second]
listen = {{ADDR[second]}}
pm = static
pm.max_children = 1
catch_workers_output = yes
decorate_workers_output = no
EOT;

$code = <<<'EOT'
<?php
if (isset($_GET['message'])) {
    file_put_contents('php://stderr', $_GET['message'] . "\n");
}
echo isset($_GET['pid']) ? getmypid() : 'ok';
fastcgi_finish_request();
$lock = fopen(__DIR__ . '/events-poll-worker-replacement.lock', 'r+');
if (!$lock || !flock($lock, LOCK_EX)) {
    throw new RuntimeException('Could not wait on worker barrier');
}
EOT;

$path = __DIR__ . '/events-poll-worker-replacement.lock';
$lock = fopen($path, 'c+');
if (!$lock || !chmod($path, 0666) || !flock($lock, LOCK_EX)) {
    die("Could not lock worker barrier\n");
}
$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$pid = (int) $tester->request('message=before&pid=1', address: '{{ADDR[first]}}')->getBody();
if ($pid <= 0) {
    $tester->terminate();
    $tester->close();
    die("Missing worker PID\n");
}
$tester->expectLogLine('before', decorated: false);
$tester->request('message=barrier', address: '{{ADDR[second]}}')->expectBody('ok');
$tester->expectLogLine('barrier', decorated: false);
$tester->signal('KILL', $pid);
$tester->expectLogWarning('child \\d+ exited on signal 9 \\(SIGKILL\\) after \\d+\\.\\d+ seconds from start', 'first');
$tester->expectLogNotice('child \\d+ started', 'first');
$tester->request('message=after', address: '{{ADDR[first]}}')->expectBody('ok');
$tester->expectLogLine('after', decorated: false);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
fclose($lock);
?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
@unlink(__DIR__ . '/events-poll-worker-replacement.lock');
?>
