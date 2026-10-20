--TEST--
Automatic worker detection is capped for regular and instrumented runs
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die('skip requires a POSIX shell');
}
?>
--ENV--
TEST_PHP_FORK_SERVER=0
--FILE--
<?php
$root = __DIR__ . '/automatic_worker_limit_' . getmypid();
$bin = $root . '/bin';
$tests = $root . '/tests';
mkdir($bin, recursive: true);
mkdir($tests);

$nproc = $bin . '/nproc';
file_put_contents($nproc, "#!/bin/sh\nprintf '64\\n'\n");
chmod($nproc, 0755);

$testFiles = [];
for ($i = 0; $i < 11; $i++) {
    $testFiles[] = $file = $tests . "/$i.phpt";
    file_put_contents($file, <<<PHPT
        --TEST--
        worker cap $i
        --FILE--
        <?php echo "ok\\n"; ?>
        --EXPECT--
        ok
        PHPT);
}

$environment = [
    'PATH' => $bin . PATH_SEPARATOR . getenv('PATH'),
    'TEST_PHP_EXECUTABLE' => getenv('TEST_PHP_EXECUTABLE'),
    'TEST_PHP_FORK_SERVER' => '0',
];
foreach (['TEMP', 'TMPDIR'] as $name) {
    if (($value = getenv($name)) !== false) {
        $environment[$name] = $value;
    }
}

$runTests = static function (array $arguments) use ($environment, $testFiles): array {
    $process = proc_open(
        [
            getenv('TEST_PHP_EXECUTABLE'),
            dirname(__DIR__, 2) . '/run-tests.php',
            '-q',
            '--no-progress',
            ...$arguments,
            ...$testFiles,
        ],
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

    return [proc_close($process), $output];
};

[$exitCode, $output] = $runTests([]);
var_dump($exitCode);
var_dump(str_contains($output, 'Spawning 10 workers...'));
var_dump(str_contains($output, 'Spawning 11 workers...'));

[$exitCode, $output] = $runTests(['--asan']);
var_dump($exitCode);
var_dump(str_contains($output, 'Spawning 2 workers...'));
var_dump(str_contains($output, 'Spawning 10 workers...'));

[$exitCode, $output] = $runTests(['--asan', '-j3']);
var_dump($exitCode);
var_dump(str_contains($output, 'Spawning 3 workers...'));
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/automatic_worker_limit_*') ?: [] as $root) {
    foreach (glob($root . '/tests/*.phpt') ?: [] as $file) {
        unlink($file);
    }
    @unlink($root . '/bin/nproc');
    @rmdir($root . '/tests');
    @rmdir($root . '/bin');
    @rmdir($root);
}
?>
--EXPECT--
int(0)
bool(true)
bool(false)
int(0)
bool(true)
bool(false)
int(0)
bool(true)
