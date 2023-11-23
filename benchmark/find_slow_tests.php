<?php

declare(strict_types=1);

use function var_dump as d;
use function arsort as arrsort;

define("IS_WINDOWS", PHP_OS_FAMILY === "Windows");
$php_binary = implode(DIRECTORY_SEPARATOR, [__DIR__, '..', 'sapi', 'cli', 'php']);
if (IS_WINDOWS) {
    $php_binary .= '.exe';
}
if (!is_executable($php_binary)) {
    echo "Error: Cannot find php binary at '{$php_binary}'\n";
    exit(1);
}
$php_binary = realpath($php_binary);
$php_src_dir = realpath(implode(DIRECTORY_SEPARATOR, [__DIR__, '..']));
$php_src_dir_relative_cutoff = strlen($php_src_dir) + strlen(DIRECTORY_SEPARATOR);
$test_files = (function () use ($php_src_dir): array {
    $test_files = [];
    $dir = new RecursiveDirectoryIterator($php_src_dir);
    $iterator = new RecursiveIteratorIterator($dir);
    foreach ($iterator as $file) {
        /** @var SplFileInfo $file */
        if ($file->getExtension() !== 'phpt') {
            continue;
        }
        $test_files[] = $file->getPathname();
    }
    return $test_files;
})();
$results_raw = [];
$cmd_template = implode(" ", array(
    escapeshellarg($php_binary),
    escapeshellarg(realpath(implode(DIRECTORY_SEPARATOR, [__DIR__, '..', 'run-tests.php']))),
//    '-p ' . escapeshellarg($php_binary),
    '--no-color',
    '--no-progress',
    '-q',
    '-x', // SKIP_SLOW_TESTS 
    '',
));
$total = count($test_files);
$null_path = IS_WINDOWS ? 'NUL' : '/dev/null'; // funfact, on Windows, every single directory has a null device file named "NUL" ...
foreach ($test_files as $test_no => $test_file) {
    $test_file_relative = substr($test_file, $php_src_dir_relative_cutoff);
    echo "Running test " . ($test_no + 1) . "/{$total}: {$test_file_relative}\n";
    $best_time = INF;
    $cmd = $cmd_template . escapeshellarg($test_file) . " > {$null_path} 2>&1";
    for ($testrun = 0; $testrun < 3; ++$testrun) {
        $start = microtime(true);
        passthru($cmd);
        $end = microtime(true);
        $best_time = min($best_time, $end - $start);
    }
    echo "Best time: {$best_time}s\n";
    $results_raw[$test_file_relative] = $best_time;
}
$results_sorted = $results_raw;
arrsort($results_sorted, SORT_NUMERIC);
var_export($results_sorted);
