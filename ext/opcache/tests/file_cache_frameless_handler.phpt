--TEST--
opcache file cache must not reuse frameless function indices across extension sets
--EXTENSIONS--
opcache
--SKIPIF--
<?php
$dl_test = PHP_OS_FAMILY === 'Windows' ? 'php_dl_test.dll' : 'dl_test.so';
if (!file_exists(ini_get('extension_dir') . DIRECTORY_SEPARATOR . $dl_test)) {
    die('skip dl_test extension is not built');
}
?>
--FILE--
<?php
$cache = __DIR__ . DIRECTORY_SEPARATOR . 'file_cache_frameless_handler';
$script = $cache . DIRECTORY_SEPARATOR . 'call.php';
mkdir($cache);
file_put_contents($script, <<<'PHP'
<?php
if (isset($argv[1])) {
    dl('dl_test');
}
try {
    var_dump(dl_test_frameless(7));
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
PHP);

function run(string $cache, array $args): void {
    $ext = ini_get('extension_dir');
    $opcache = PHP_OS_FAMILY === 'Windows' ? 'php_opcache.dll' : 'opcache.so';
    $cmd = [PHP_BINARY, '-n', '-d', 'extension_dir="' . $ext . '"'];
    if (file_exists($ext . DIRECTORY_SEPARATOR . $opcache)) {
        array_push($cmd, '-d', 'zend_extension=opcache');
    }
    array_push($cmd,
        '-d', 'opcache.enable_cli=1',
        '-d', 'opcache.file_cache="' . $cache . '"',
        '-d', 'opcache.file_cache_only=1',
        '-d', 'opcache.file_update_protection=0',
        ...$args,
    );
    $proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    echo stream_get_contents($pipes[1]);
    stream_get_contents($pipes[2]);
    proc_close($proc);
}

function system_ids(string $cache): int {
    $pattern = PHP_OS_FAMILY === 'Windows' ? '/*/*' : '/*';
    return count(glob($cache . $pattern, GLOB_ONLYDIR));
}

run($cache, ['-d', 'extension=dl_test', $script]);
var_dump(system_ids($cache));
run($cache, [$script, 'dl']);
var_dump(system_ids($cache));
run($cache, [$script]);
var_dump(system_ids($cache));
?>
--CLEAN--
<?php
$cache = __DIR__ . DIRECTORY_SEPARATOR . 'file_cache_frameless_handler';
if (is_dir($cache)) {
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($cache, FilesystemIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($it as $file) {
        $file->isDir() ? rmdir($file->getPathname()) : unlink($file->getPathname());
    }
    rmdir($cache);
}
?>
--EXPECT--
int(7)
int(1)
int(7)
int(2)
Error: Call to undefined function dl_test_frameless()
int(2)
