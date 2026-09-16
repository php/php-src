--TEST--
UserCache\Cache: the "posix" memory model commits its object up front and yields to the next backend when /dev/shm is too small
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip POSIX shm objects are size-capped by a tmpfs on Linux only');
if (!is_dir('/dev/shm') || !is_writable('/dev/shm')) die('skip /dev/shm not writable');
$free = @disk_free_space('/dev/shm');
if ($free === false || (int) ceil($free / (1024 * 1024)) + 64 > 3584) {
    die('skip /dev/shm too large to exhaust within the shm_size limit');
}
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$probe = 'UserCache\Cache::getPool("probe")->store("k", 1);'
    . 'ob_start(); phpinfo(INFO_MODULES);'
    . 'preg_match("/^Active memory model => (.+)$/m", ob_get_clean(), $m);'
    . 'echo trim($m[1] ?? "none");';
$cmd = escapeshellarg($php)
    . ' -n -d user_cache.enable=1 -d user_cache.enable_cli=1 -d user_cache.shm_size=16M'
    . ' -d user_cache.preferred_memory_model=posix'
    . ' -r ' . escapeshellarg($probe) . ' 2>/dev/null';
$active = trim((string) shell_exec($cmd));
if ($active !== 'posix') {
    die("skip posix memory model unavailable (active: $active)");
}
?>
--FILE--
<?php

$free = disk_free_space('/dev/shm');
$shmSize = ((int) ceil($free / (1024 * 1024)) + 64) . 'M';

$script = <<<'PHP'
$cache = UserCache\Cache::getPool('nospace');
/* Would fault an unbacked tmpfs page on a sparse object. */
for ($i = 0; $i < 96; $i++) {
    if (!$cache->store("key$i", str_repeat('x', 1024 * 1024))) {
        break;
    }
}
ob_start();
phpinfo(INFO_MODULES);
preg_match('/^Active memory model => (.+)$/m', ob_get_clean(), $m);
echo UserCache\Cache::getStatus()->getAvailability()->name, ':', trim($m[1] ?? 'none'), ':', is_string($cache->fetch('key0')) ? 'HIT' : 'MISS', "\n";
PHP;

$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
$cmd = escapeshellarg($php)
    . ' -n -d user_cache.enable=1 -d user_cache.enable_cli=1'
    . ' -d user_cache.shm_size=' . $shmSize
    . ' -d user_cache.preferred_memory_model=posix'
    . ' -d display_errors=1 -d log_errors=0 -d html_errors=0'
    . ' -r ' . escapeshellarg($script) . ' 2>&1';

$output = trim((string) shell_exec($cmd));
/* The oversized posix object is refused at creation (never mapped sparse),
 * the built-in order then falls through to the next backend silently. */
var_dump(str_contains($output, 'SIGBUS') || str_contains($output, 'Bus error'));
var_dump(preg_match('/^Available:(?!posix)[a-z0-9]+:HIT$/', $output) === 1);
var_dump(scandir('/dev/shm') !== false && count(preg_grep('/^php_uc\./', scandir('/dev/shm'))) === 0);

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
