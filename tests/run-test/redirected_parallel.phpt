--TEST--
Redirected tests work in parallel runs and update the progress total
--ENV--
TEST_PHP_FORK_SERVER=0
--FILE--
<?php
function writeRedirectedTest(string $file, string $name): void
{
    file_put_contents($file, <<<PHPT
        --TEST--
        $name
        --FILE--
        <?php echo "ok\\n"; ?>
        --EXPECT--
        ok
        PHPT);
}

function runRedirectedTests(array $testFiles): array
{
    $command = [
        getenv('TEST_PHP_EXECUTABLE'),
        '-n',
        dirname(__DIR__, 2) . '/run-tests.php',
        '-n',
        '-q',
        '-j2',
        '--progress',
        ...$testFiles,
    ];
    $environment = [
        'PATH' => getenv('PATH'),
        'TEST_PHP_EXECUTABLE' => getenv('TEST_PHP_EXECUTABLE'),
        'TEST_PHP_FORK_SERVER' => '0',
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
    return [proc_close($process), str_replace("\r", "\n", $output)];
}

$root = __DIR__ . '/redirected_parallel_' . getmypid();
$targets = $root . '/targets';
mkdir($targets, recursive: true);

writeRedirectedTest($targets . '/one.phpt', 'redirected one');
writeRedirectedTest($targets . '/two.phpt', 'redirected two');

$targetExpression = var_export($targets, true);
$redirect = $root . '/redirect.phpt';
file_put_contents($redirect, <<<PHPT
    --TEST--
    redirect wrapper
    --REDIRECTTEST--
    return ['ENV' => [], 'TESTS' => $targetExpression];
    PHPT);
writeRedirectedTest($root . '/companion.phpt', 'companion');

[$singleExitCode, $singleOutput] = runRedirectedTests([$redirect]);
if ($singleExitCode !== 0) {
    echo $singleOutput;
}
var_dump($singleExitCode);
var_dump(str_contains($singleOutput, 'Fatal error'));

[$parallelExitCode, $parallelOutput] = runRedirectedTests([
    $redirect,
    $root . '/companion.phpt',
]);
if ($parallelExitCode !== 0) {
    echo $parallelOutput;
}
var_dump($parallelExitCode);
var_dump(str_contains($parallelOutput, 'TEST 3/3'));
var_dump(str_contains($parallelOutput, 'TEST 3/2'));
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/redirected_parallel_*') ?: [] as $root) {
    foreach (glob($root . '/targets/*') ?: [] as $file) {
        unlink($file);
    }
    @rmdir($root . '/targets');
    @unlink($root . '/redirect.phpt');
    @unlink($root . '/companion.phpt');
    @rmdir($root);
}
?>
--EXPECT--
int(0)
bool(false)
int(0)
bool(true)
bool(false)
