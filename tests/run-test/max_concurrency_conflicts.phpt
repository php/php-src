--TEST--
MAX_CONCURRENCY does not weaken directory conflicts
--ENV--
TEST_PHP_FORK_SERVER=0
--FILE--
<?php
function writeParallelTest(string $file, string $name, string $code): void
{
    file_put_contents($file, <<<PHPT
        --TEST--
        $name
        --FILE--
        <?php
        $code
        ?>
        --EXPECT--
        ok
        PHPT);
}

$root = __DIR__ . '/max_concurrency_conflicts_' . getmypid();
$results = $root . '/results.txt';
mkdir($root);
file_put_contents($root . '/CONFLICTS', "shared\n");
file_put_contents($root . '/MAX_CONCURRENCY', "2\n");

$rootExpression = var_export($root, true);
$testCode = <<<'PHP'
    $root = %s;
    $marker = $root . '/active-' . getmypid();
    file_put_contents($marker, '');
    $deadline = microtime(true) + 0.5;
    do {
        $overlap = count(glob($root . '/active-*')) > 1;
        if ($overlap) {
            break;
        }
        usleep(10_000);
    } while (microtime(true) < $deadline);
    unlink($marker);
    echo $overlap ? "overlap\n" : "ok\n";
    PHP;
$testCode = sprintf($testCode, $rootExpression);
writeParallelTest($root . '/one.phpt', 'conflict one', $testCode);
writeParallelTest($root . '/two.phpt', 'conflict two', $testCode);

$command = [
    getenv('TEST_PHP_EXECUTABLE'),
    dirname(__DIR__, 2) . '/run-tests.php',
    '-q',
    '-j2',
    '-W',
    $results,
    $root . '/one.phpt',
    $root . '/two.phpt',
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
$exitCode = proc_close($process);

$resultLines = file($results, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
sort($resultLines);
foreach ($resultLines as $resultLine) {
    [$status, $file] = explode("\t", $resultLine, 2);
    echo "$status ", basename($file), "\n";
}
var_dump($exitCode);
if ($exitCode !== 0) {
    echo $output;
}

foreach (glob($root . '/*') as $file) {
    unlink($file);
}
rmdir($root);
?>
--EXPECT--
PASSED one.phpt
PASSED two.phpt
int(0)
