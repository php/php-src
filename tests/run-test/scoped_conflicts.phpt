--TEST--
Scoped directory conflicts allow internal concurrency
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

$root = __DIR__ . '/scoped_conflicts_' . getmypid();
$group = $root . '/a-group';
$external = $root . '/z-external';
$results = $root . '/results.txt';
mkdir($group, recursive: true);
mkdir($external);
file_put_contents($group . '/SCOPED_CONFLICTS', "shared\n");
file_put_contents($group . '/MAX_CONCURRENCY', "2\n");

$rootExpression = var_export($root, true);
$groupCode = <<<'PHP'
    $root = %s;
    $marker = $root . '/active-group-' . getmypid();
    file_put_contents($marker, '');
    $deadline = microtime(true) + 10;
    do {
        $groupIsParallel = count(glob($root . '/active-group-*')) === 2;
        if ($groupIsParallel) {
            break;
        }
        usleep(10_000);
    } while (microtime(true) < $deadline);
    if ($groupIsParallel) {
        // Give the peer time to observe both markers before either is removed.
        usleep(100_000);
    }
    $externalOverlaps = file_exists($root . '/active-external');
    unlink($marker);
    echo $groupIsParallel && !$externalOverlaps ? "ok\n" : "not isolated\n";
    PHP;
$groupCode = sprintf($groupCode, $rootExpression);
writeParallelTest($group . '/one.phpt', 'scoped conflict one', $groupCode);
writeParallelTest($group . '/two.phpt', 'scoped conflict two', $groupCode);

$externalCode = <<<PHP
    \$root = $rootExpression;
    \$marker = \$root . '/active-external';
    file_put_contents(\$marker, '');
    \$groupOverlaps = glob(\$root . '/active-group-*');
    unlink(\$marker);
    echo \$groupOverlaps ? "not isolated\n" : "ok\n";
    PHP;
file_put_contents($external . '/external.phpt', <<<PHPT
    --TEST--
    external conflict
    --CONFLICTS--
    shared
    --FILE--
    <?php
    $externalCode
    ?>
    --EXPECT--
    ok
    PHPT);

$command = [
    getenv('TEST_PHP_EXECUTABLE'),
    dirname(__DIR__, 2) . '/run-tests.php',
    '-q',
    '-j3',
    '-W',
    $results,
    $group . '/one.phpt',
    $group . '/two.phpt',
    $external . '/external.phpt',
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

foreach (glob($root . '/active-*') as $marker) {
    unlink($marker);
}
unlink($results);
foreach ([$group, $external] as $directory) {
    foreach (glob($directory . '/*') as $file) {
        unlink($file);
    }
}
rmdir($group);
rmdir($external);
rmdir($root);
?>
--EXPECT--
PASSED one.phpt
PASSED two.phpt
PASSED external.phpt
int(0)
