--TEST--
FPM: GHSA-g3vh-838c-65v7 terminates a worker after interrupted session shutdown
--EXTENSIONS--
session
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
require_once "tester.inc";

$sessionDir = __DIR__ . '/ghsa-g3vh-838c-65v7.sessions';
mkdir($sessionDir, 0700);

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
log_level = notice
[unconfined]
listen = {{ADDR:UDS}}
pm = static
pm.max_children = 1
php_admin_value[session.save_path] = $sessionDir
php_admin_flag[opcache.enable] = off
php_admin_flag[display_errors] = off
php_admin_flag[log_errors] = off
EOT;

$code = <<<'PHP'
<?php
if ($_SERVER['QUERY_STRING'] !== 'producer') {
    echo getmypid();
    return;
}

class ShutdownStream {
    public $context;
    private $fp;

    public function stream_open($path, $mode, $options, &$openedPath) {
        $this->fp = fopen('/dev/null', 'w');
        return true;
    }

    public function stream_write($data) {
        return strlen($data);
    }

    public function stream_close() {
        trigger_error('session shutdown bailout', E_USER_ERROR);
    }
}

stream_wrapper_register('shutdown-test', ShutdownStream::class);
session_id('g3vh' . getmypid());
session_start();
$_SESSION['stream'] = fopen('shutdown-test://session', 'w');
unset($_SESSION);

// Finish the response before the fatal RSHUTDOWN callback runs.
echo 'producer';
fastcgi_finish_request();
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(extensions: ['session']);
$tester->expectLogStartNotices();
$pid = (int) $tester->request(address: '{{ADDR:UDS}}')->getBody();
$tester->request(query: 'producer', address: '{{ADDR:UDS}}')->expectBody('producer');
$tester->expectLogWarning(
    'child ' . $pid . ' exited with code 1 after \\d+\\.\\d+ seconds from start',
    'unconfined',
    timeoutSeconds: 5
);
$tester->expectLogNotice('child \\d+ started', 'unconfined');
$replacement = (int) $tester->request(address: '{{ADDR:UDS}}')->getBody();
var_dump($pid > 0 && $replacement > 0 && $replacement !== $pid);
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
?>
Done
--EXPECT--
bool(true)
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
$sessionDir = __DIR__ . '/ghsa-g3vh-838c-65v7.sessions';
foreach (glob($sessionDir . '/sess_*') as $file) {
    unlink($file);
}
@rmdir($sessionDir);
?>
