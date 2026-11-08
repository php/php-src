--TEST--
Shutdown of a process that fell back to the file cache must not free the SHM globals
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
$dll = 'php' . PHP_MAJOR_VERSION . (PHP_ZTS ? 'ts' : '') . (PHP_DEBUG ? '_debug' : '') . '.dll';
if (!file_exists(dirname(PHP_BINARY) . DIRECTORY_SEPARATOR . $dll)) die("skip $dll not found next to PHP_BINARY");
?>
--FILE--
<?php
/* This process holds the SHM. A child run from a copy of the PHP binary
 * and core DLL gets the core DLL at another address (ASLR), so it cannot
 * reattach and falls back to the file cache. */
$dll = 'php' . PHP_MAJOR_VERSION . (PHP_ZTS ? 'ts' : '') . (PHP_DEBUG ? '_debug' : '') . '.dll';
$srcDir = dirname(PHP_BINARY);
$base = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'opcache_file_cache_fallback_win32';
$binDir = $base . DIRECTORY_SEPARATOR . 'bin';
$cacheDir = $base . DIRECTORY_SEPARATOR . 'cache';
@mkdir($binDir, 0777, true);
@mkdir($cacheDir, 0777, true);
copy(PHP_BINARY, $binDir . DIRECTORY_SEPARATOR . basename(PHP_BINARY));
copy($srcDir . DIRECTORY_SEPARATOR . $dll, $binDir . DIRECTORY_SEPARATOR . $dll);

$args = [
    $binDir . DIRECTORY_SEPARATOR . basename(PHP_BINARY),
    '-n',
    '-d', 'extension_dir=' . ini_get('extension_dir'),
];
if (file_exists(ini_get('extension_dir') . DIRECTORY_SEPARATOR . 'php_opcache.dll')) {
    $args[] = '-d';
    $args[] = 'zend_extension=opcache';
}
foreach (['opcache.memory_consumption', 'opcache.jit', 'opcache.jit_buffer_size'] as $name) {
    $args[] = '-d';
    $args[] = $name . '=' . ini_get($name);
}
array_push($args,
    '-d', 'opcache.enable=1',
    '-d', 'opcache.enable_cli=1',
    '-d', 'opcache.file_cache=' . $cacheDir,
    '-d', 'opcache.file_cache_fallback=1',
    '-r', 'var_dump(opcache_get_status(false)["file_cache_only"]);'
);

/* Other DLLs the core DLL depends on are still found next to PHP_BINARY. */
$env = getenv();
$env['PATH'] = $srcDir . PATH_SEPARATOR . ($env['PATH'] ?? '');

$proc = proc_open($args, [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes, null, $env);
echo stream_get_contents($pipes[1]);
echo stream_get_contents($pipes[2]);
printf("exit code: 0x%08X\n", proc_close($proc) & 0xFFFFFFFF);
?>
--CLEAN--
<?php
$base = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'opcache_file_cache_fallback_win32';
foreach (new RecursiveIteratorIterator(new RecursiveDirectoryIterator($base, FilesystemIterator::SKIP_DOTS), RecursiveIteratorIterator::CHILD_FIRST) as $f) {
    $f->isDir() ? @rmdir($f->getPathname()) : @unlink($f->getPathname());
}
@rmdir($base);
?>
--EXPECT--
bool(true)
exit code: 0x00000000
