--TEST--
UserCache\Cache: the minimum shm_size reported by the startup warning does not depend on shm_size and is exactly the smallest segment that initializes with room for one block
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$args = ['-n', '-d', 'display_errors=1', '-d', 'display_startup_errors=1', '-d', 'error_reporting=E_ALL', '-d', 'user_cache.enable=1', '-d', 'user_cache.enable_cli=1'];
$probe = 'echo UserCache\\Cache::getStatus()->getAvailability()->name;';
$run = function (int $size) use ($php, $args, $probe): string {
    $process = proc_open([$php, ...$args, '-d', "user_cache.shm_size=$size", '-r', $probe], [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    $output = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    proc_close($process);

    return trim($output);
};

preg_match('/minimum cache layout \((\d+) bytes\)/', $run(16), $m);
$minimum = (int) $m[1];
var_dump($minimum > 16);

$belowMinimum = $run($minimum - 1);
var_dump(str_contains($belowMinimum, "is below the minimum cache layout ($minimum bytes)"), str_ends_with($belowMinimum, 'UnavailableBySharedMemoryInitializationFailed'));
var_dump($run($minimum));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(9) "Available"
