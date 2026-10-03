--TEST--
UserCache\Cache: a preferred_memory_model that is not available on the platform and a lockfile_path that is not absolute are rejected with a warning and keep their defaults
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip user_cache.lockfile_path is only used and validated on POSIX systems');
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.preferred_memory_model=bogus
user_cache.lockfile_path=relative/dir
--FILE--
<?php
var_dump(ini_get('user_cache.preferred_memory_model'), ini_get('user_cache.lockfile_path'));
var_dump(UserCache\Cache::getPool('ini-validation')->store('k', 1), UserCache\Cache::getPool('ini-validation')->fetch('k'));

$run = function (array $args): string {
    $process = proc_open([PHP_BINARY, '-n', ...$args], [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    $output = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    proc_close($process);

    return $output;
};

/* "cgi" is accepted as an alias of "shm", which only builds with SysV shared memory provide (never macOS) */
$shm = $run(['-d', 'user_cache.preferred_memory_model=shm', '-r', 'echo ini_get("user_cache.preferred_memory_model");']);
$model = $shm === 'shm' ? 'cgi' : 'mmap';
echo $run([
    '-d', 'user_cache.enable=1',
    '-d', 'user_cache.enable_cli=1',
    '-d', "user_cache.preferred_memory_model=$model",
    '-d', 'user_cache.lockfile_path=',
    '-r', 'var_dump(ini_get("user_cache.preferred_memory_model"), ini_get("user_cache.lockfile_path"), UserCache\Cache::getPool("x")->store("k", 1));',
]);
?>
--EXPECTF--
Warning: user_cache.lockfile_path must be an absolute path, "relative/dir" given in Unknown on line 0

Warning: user_cache.preferred_memory_model "bogus" is not a memory model available on this platform in Unknown on line 0
string(0) ""
string(4) "/tmp"
bool(true)
int(1)

Warning: user_cache.lockfile_path must be an absolute path, "" given in Unknown on line 0
string(%d) "%s"
string(4) "/tmp"
bool(true)
