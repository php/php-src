--TEST--
GH-25077: FPM captured worker output reaches syslog
--SKIPIF--
<?php
require __DIR__ . '/skipif.inc';
if (PHP_OS_FAMILY !== 'Linux') {
    die('skip Linux LD_PRELOAD test');
}
if (PHP_INT_SIZE !== 8) {
    die('skip 64-bit test shim only');
}
if (!function_exists('exec')) {
    die('skip exec is disabled');
}
exec('command -v cc 2>/dev/null', $output, $status);
if ($status !== 0 || !$output) {
    die('skip C compiler not available');
}
?>
--FILE--
<?php
require_once __DIR__ . '/tester.inc';

$temp = tempnam(sys_get_temp_dir(), 'fpm-gh25077-');
$library = $temp . '.so';
$log = $temp . '.log';
$source = __DIR__ . '/gh25077_syslog_intercept.c';

try {
    exec('cc -shared -fPIC -Wall -Wextra -Werror -o ' . escapeshellarg($library)
        . ' ' . escapeshellarg($source) . ' 2>&1', $output, $status);
    if ($status !== 0) {
        throw new RuntimeException('Failed to compile test syslog interceptor: ' . implode("\n", $output));
    }

    $config = <<<'CONFIG'
[global]
error_log = syslog
daemonize = no
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
catch_workers_output = yes
CONFIG;

    $code = <<<'PHP'
<?php
file_put_contents('php://stdout', "GH25077_WORKER_STDOUT\n");
file_put_contents('php://stderr', "GH25077_WORKER_STDERR\n");
PHP;

    $env = getenv();
    $env['LD_PRELOAD'] = (isset($env['LD_PRELOAD']) ? $env['LD_PRELOAD'] . ' ' : '') . $library;
    $env['PHP_FPM_TEST_SYSLOG_CAPTURE'] = $log;

    $tester = new FPM\Tester($config, $code);
    $tester->start(forceStderr: false, envVars: $env);
    $tester->checkConnection();
    $tester->request()->expectEmptyBody();

    $found = false;
    for ($i = 0; $i < 100; $i++) {
        if (is_file($log)) {
            $captured = file_get_contents($log);
            if (str_contains($captured, 'GH25077_WORKER_STDOUT')
                && str_contains($captured, 'GH25077_WORKER_STDERR')) {
                $found = true;
                break;
            }
        }
        usleep(50000);
    }
    $tester->terminate();
    $tester->close();
    var_dump($found);
} finally {
    @unlink($log);
    @unlink($library);
    @unlink($temp);
}
?>
Done
--EXPECT--
bool(true)
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>
