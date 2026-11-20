--TEST--
GH-25555: Scripts loaded from file_cache into SHM report memory_consumption 0 when stored without SHM
--EXTENSIONS--
opcache
--FILE--
<?php
$cache = __DIR__ . DIRECTORY_SEPARATOR . 'gh25555_cache';
$script = $cache . DIRECTORY_SEPARATOR . 'target.php';
mkdir($cache);
file_put_contents($script, <<<'PHP'
<?php
function gh25555_test(): int {
    return 42;
}
PHP);

function run_child(string $cache, string $script, bool $file_cache_only): void {
    $ext = ini_get('extension_dir');
    $opcache = PHP_OS_FAMILY === 'Windows' ? 'php_opcache.dll' : 'opcache.so';
    $cmd = [PHP_BINARY, '-n', '-d', 'extension_dir="' . $ext . '"'];
    if (file_exists($ext . DIRECTORY_SEPARATOR . $opcache)) {
        array_push($cmd, '-d', 'zend_extension=opcache');
    }
    $code = sprintf(
        'opcache_compile_file(%s);'
        . ' if (!%s) {'
        . '   $s = opcache_get_status(true);'
        . '   $mem = $s["scripts"][%s]["memory_consumption"];'
        . '   $w0 = $s["memory_usage"]["wasted_memory"];'
        . '   opcache_invalidate(%s, true);'
        . '   $w1 = opcache_get_status(false)["memory_usage"]["wasted_memory"];'
        . '   var_dump($mem > 0, $w1 - $w0 === $mem);'
        . ' }',
        var_export($script, true),
        $file_cache_only ? 'true' : 'false',
        var_export($script, true),
        var_export($script, true),
    );
    array_push($cmd,
        '-d', 'opcache.enable=1',
        '-d', 'opcache.enable_cli=1',
        '-d', 'opcache.jit=0',
        '-d', 'opcache.file_cache="' . $cache . '"',
        '-d', 'opcache.file_cache_only=' . ($file_cache_only ? '1' : '0'),
        '-d', 'opcache.file_update_protection=0',
        '-r', $code,
    );
    $proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    echo stream_get_contents($pipes[1]);
    proc_close($proc);
}

run_child($cache, $script, true);
run_child($cache, $script, false);
?>
--CLEAN--
<?php
$cache = __DIR__ . DIRECTORY_SEPARATOR . 'gh25555_cache';
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
bool(true)
bool(true)
