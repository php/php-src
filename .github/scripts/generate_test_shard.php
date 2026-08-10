<?php

if ($argc !== 4) {
    fwrite(STDERR, "Usage: php generate_test_shard.php <shard> <shards> <output>\n");
    exit(1);
}

$shard = filter_var($argv[1], FILTER_VALIDATE_INT, ['options' => ['min_range' => 1]]);
$shards = filter_var($argv[2], FILTER_VALIDATE_INT, ['options' => ['min_range' => 1]]);
if ($shard === false || $shards === false || $shard > $shards) {
    fwrite(STDERR, "The shard must be between 1 and the total number of shards.\n");
    exit(1);
}

$root = dirname(__DIR__, 2);
$extensions = get_loaded_extensions();
$extensionDir = ini_get('extension_dir');
if (is_dir($extensionDir)) {
    foreach (scandir($extensionDir) as $file) {
        if (preg_match('/^(?:php_)?([_a-zA-Z0-9]+)\.' . preg_quote(PHP_SHLIB_SUFFIX, '/') . '$/', $file, $matches)) {
            $extensions[] = $matches[1];
        }
    }
}
$extensions = array_map(
    static fn(string $extension): string => $extension === 'Zend OPcache' ? 'opcache' : strtolower($extension),
    $extensions,
);
$extensions = array_fill_keys($extensions, true);
unset($extensions['core']);

$tests = [];
$collectTests = static function (string $directory) use (&$tests): void {
    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($directory, FilesystemIterator::SKIP_DOTS),
    );
    foreach ($iterator as $file) {
        if ($file->isFile() && $file->getExtension() === 'phpt' && $file->getFilename()[0] !== '.') {
            $tests[] = $file->getRealPath();
        }
    }
};

foreach (['Zend', 'tests', 'sapi'] as $directory) {
    if (is_dir("$root/$directory")) {
        $collectTests("$root/$directory");
    }
}
foreach (new DirectoryIterator("$root/ext") as $extensionDirectory) {
    if ($extensionDirectory->isDir()
        && !$extensionDirectory->isDot()
        && isset($extensions[strtolower($extensionDirectory->getFilename())])
    ) {
        $collectTests($extensionDirectory->getPathname());
    }
}

sort($tests, SORT_STRING);
$testShards = array_fill(0, $shards, []);
foreach ($tests as $index => $test) {
    $testShards[$index % $shards][] = $test;
}
$partitionedTests = array_merge(...$testShards);
if (count($partitionedTests) !== count($tests)
    || count(array_unique($partitionedTests)) !== count($tests)
) {
    fwrite(STDERR, "Test shard partition is incomplete or contains duplicates.\n");
    exit(1);
}
$selectedTests = $testShards[$shard - 1];
if ($selectedTests === []) {
    fwrite(STDERR, "Shard $shard of $shards contains no tests.\n");
    exit(1);
}

$output = $argv[3];
if (file_put_contents($output, implode(PHP_EOL, $selectedTests) . PHP_EOL) === false) {
    fwrite(STDERR, "Failed to write test shard to $output.\n");
    exit(1);
}

printf("Selected %d of %d tests for shard %d of %d.\n", count($selectedTests), count($tests), $shard, $shards);
