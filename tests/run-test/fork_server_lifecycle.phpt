--TEST--
Test fork server timeout, termination, and failure fallback
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die('skip requires fork');
}

$process = proc_open(
    [getenv('TEST_PHP_EXECUTABLE'), '-n', '--test-fork-server', 'TESTTOKEN'],
    [
        0 => ['pipe', 'r'],
        1 => ['pipe', 'w'],
        2 => ['redirect', 1],
    ],
    $pipes,
);
fclose($pipes[0]);
stream_get_contents($pipes[1]);
fclose($pipes[1]);
if (proc_close($process) !== 0) {
    die('skip fork server is not available');
}
?>
--ENV--
TEST_PHP_FORK_SERVER=0
--FILE--
<?php
function writeTest(string $file, string $name, string $code, string $expected): void
{
    file_put_contents($file, <<<PHPT
        --TEST--
        $name
        --EXTENSIONS--
        posix
        --FILE--
        <?php
        $code
        ?>
        --EXPECT--
        $expected
        PHPT);
}

/**
 * @return array{int, string, array<string, string>}
 */
function runTests(array $files, string $results, array $arguments = []): array
{
    $command = [
        getenv('TEST_PHP_EXECUTABLE'),
        '-n',
        dirname(__DIR__, 2) . '/run-tests.php',
        '-n',
        '-q',
        '-j1',
        ...$arguments,
        '-W',
        $results,
        ...$files,
    ];
    $environment = [
        'PATH' => getenv('PATH'),
        'TEST_PHP_EXECUTABLE' => getenv('TEST_PHP_EXECUTABLE'),
        'TEST_PHP_FORK_SERVER' => '1',
    ];
    foreach (['SystemRoot', 'TEMP', 'TMPDIR'] as $name) {
        if (($value = getenv($name)) !== false) {
            $environment[$name] = $value;
        }
    }
    $process = proc_open(
        $command,
        [
            0 => ['pipe', 'r'],
            1 => ['pipe', 'w'],
            2 => ['redirect', 1],
        ],
        $pipes,
        null,
        $environment,
    );
    fclose($pipes[0]);
    $output = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    $exitCode = proc_close($process);

    $statuses = [];
    foreach (file($results, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) as $resultLine) {
        [$status, $file] = explode("\t", $resultLine, 2);
        $statuses[basename($file)] = $status;
    }
    ksort($statuses);
    return [$exitCode, $output, $statuses];
}

function processStopped(int $pid, string $heartbeat): bool
{
    if (!@posix_kill($pid, 0)) {
        return true;
    }

    clearstatcache(true, $heartbeat);
    $size = @filesize($heartbeat);
    for ($attempt = 0; $attempt < 4; $attempt++) {
        usleep(500_000);
        clearstatcache(true, $heartbeat);
        $nextSize = @filesize($heartbeat);
        if ($size !== false && $size === $nextSize) {
            return true;
        }
        $size = $nextSize;
    }

    return false;
}

$root = __DIR__ . '/fork_server_lifecycle_' . getmypid();
mkdir($root);

$timeoutPid = $root . '/timeout.pid';
$timeoutPidExpression = var_export($timeoutPid, true);
$timeoutHeartbeat = $root . '/timeout.heartbeat';
$timeoutHeartbeatExpression = var_export($timeoutHeartbeat, true);
$timeoutTest = $root . '/01-timeout.phpt';
$afterTest = $root . '/02-after.phpt';
writeTest(
    $timeoutTest,
    'fork server timeout',
    <<<PHP
        file_put_contents($timeoutPidExpression, getmypid());
        while (true) {
            file_put_contents($timeoutHeartbeatExpression, '.', FILE_APPEND);
            usleep(10_000);
        }
        PHP,
    '** ERROR: process timed out **',
);
writeTest($afterTest, 'run after timeout', 'echo "after\n";', 'after');
$timeoutResults = $root . '/timeout-results.txt';
[$timeoutExit, $timeoutOutput, $timeoutStatuses] = runTests(
    [$timeoutTest, $afterTest],
    $timeoutResults,
    ['--set-timeout', '2'],
);
foreach ($timeoutStatuses as $file => $status) {
    echo "timeout $status $file\n";
}
var_dump($timeoutExit);

$timeoutChildStopped = false;
if (file_exists($timeoutPid) && file_exists($timeoutHeartbeat)) {
    $timeoutChild = (int) file_get_contents($timeoutPid);
    $timeoutChildStopped = processStopped($timeoutChild, $timeoutHeartbeat);
    if (!$timeoutChildStopped) {
        posix_kill($timeoutChild, 9);
    }
}
echo $timeoutChildStopped ? "timeout child terminated\n" : "timeout child leaked\n";
if ($timeoutExit !== 0 || count($timeoutStatuses) !== 2) {
    echo $timeoutOutput;
}

$failureMarker = $root . '/failure.marker';
$failureMarkerExpression = var_export($failureMarker, true);
$failureTest = $root . '/failure.phpt';
writeTest(
    $failureTest,
    'fork server failure fallback',
    <<<PHP
        if (!file_exists($failureMarkerExpression)) {
            file_put_contents($failureMarkerExpression, '');
            posix_kill(posix_getppid(), 9);
            usleep(100_000);
            exit;
        }
        echo "fallback\n";
        PHP,
    'fallback',
);
$failureResults = $root . '/failure-results.txt';
[$failureExit, $failureOutput, $failureStatuses] = runTests(
    [$failureTest],
    $failureResults,
);
foreach ($failureStatuses as $file => $status) {
    echo "failure $status $file\n";
}
var_dump($failureExit);
if ($failureExit !== 0 || count($failureStatuses) !== 1) {
    echo $failureOutput;
}
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/fork_server_lifecycle_*') ?: [] as $root) {
    foreach (glob($root . '/*') ?: [] as $file) {
        @unlink($file);
    }
    @rmdir($root);
}
?>
--EXPECT--
timeout PASSED 01-timeout.phpt
timeout PASSED 02-after.phpt
int(0)
timeout child terminated
failure PASSED failure.phpt
int(0)
