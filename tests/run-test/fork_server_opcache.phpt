--TEST--
Test fork server is disabled when CLI OPcache is enabled
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows' || PHP_ZTS) {
    die('skip requires a non-ZTS build with fork');
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

$process = proc_open(
    [
        getenv('TEST_PHP_EXECUTABLE'),
        '-n',
        '-r',
        'exit(extension_loaded("Zend OPcache") ? 0 : 1);',
    ],
    [],
    $pipes,
);
if (proc_close($process) !== 0) {
    die('skip requires OPcache without additional arguments');
}
?>
--ENV--
TEST_PHP_FORK_SERVER=0
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$process = proc_open(
    [
        $php,
        '-n',
        '-d',
        'opcache.enable=1',
        '-d',
        'opcache.enable_cli=1',
        '--test-fork-server',
        'TESTTOKEN',
    ],
    [
        0 => ['pipe', 'r'],
        1 => ['pipe', 'w'],
        2 => ['redirect', 1],
    ],
    $pipes,
);
fclose($pipes[0]);
echo stream_get_contents($pipes[1]);
fclose($pipes[1]);
var_dump(proc_close($process));

$results = __DIR__ . '/fork_server_opcache_results.txt';
$command = [
    $php,
    dirname(__DIR__, 2) . '/run-tests.php',
    '-q',
    '-j1',
    '-d',
    'opcache.enable=1',
    '-d',
    'opcache.enable_cli=1',
    '-W',
    $results,
    dirname(__DIR__, 2) . '/ext/opcache/tests/gh17422/001.phpt',
    dirname(__DIR__, 2) . '/ext/opcache/tests/gh17422/003.phpt',
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

foreach (file($results, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) as $resultLine) {
    [$status, $file] = explode("\t", $resultLine, 2);
    echo $status, ' ', basename($file), "\n";
}
var_dump($exitCode);
if ($exitCode !== 0) {
    echo $output;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/fork_server_opcache_results.txt');
?>
--EXPECT--
--test-fork-server cannot be used when opcache.enable_cli is enabled.
int(1)
PASSED 001.phpt
PASSED 003.phpt
int(0)
