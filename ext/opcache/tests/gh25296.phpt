--TEST--
GH-25296 (Segfault or wrong function called with dl() and opcache.file_cache)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
$dl_test = PHP_OS_FAMILY === 'Windows' ? 'php_dl_test.dll' : 'dl_test.so';
if (!file_exists(ini_get('extension_dir') . '/' . $dl_test)) {
    die('skip dl_test extension is not built');
}
?>
--FILE--
<?php
$cache = __DIR__ . '/' . 'gh25296';
$main = $cache . '/' . 'main.php';
$call = $cache . '/' . 'call.php';
mkdir($cache);

file_put_contents($main, <<<'PHP'
<?php
set_exception_handler(function (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
});
if ($argv[1] === 'shifted') {
    function a() {}
    function b() {}
}
if ($argv[1] === 'user') {
    function dl_test_test2(string $str): string {
        return "User $str";
    }
} elseif ($argv[1] !== 'missing') {
    dl('dl_test');
}
include __DIR__ . '/call.php';
PHP);

function write_script(string $script, int $version): void {
    file_put_contents($script, <<<PHP
    <?php
    echo "v$version\n";
    echo dl_test_test2("hi"), "\n";
    PHP);
}

function run(string $cache, string $script, string $mode): void {
    $ext = ini_get('extension_dir');
    $opcache = PHP_OS_FAMILY === 'Windows' ? 'php_opcache.dll' : 'opcache.so';
    $cmd = [PHP_BINARY, '-n', '-d', 'extension_dir="' . $ext . '"'];
    if (file_exists($ext . '/' . $opcache)) {
        array_push($cmd, '-d', 'zend_extension=opcache');
    }
    array_push($cmd,
        '-d', 'opcache.enable_cli=1',
        '-d', 'opcache.file_cache="' . $cache . '"',
        '-d', 'opcache.file_cache_only=1',
        '-d', 'opcache.file_update_protection=0',
        '-d', 'opcache.validate_timestamps=0', // Important to tell apart versions
        $script,
        $mode,
    );
    $proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    echo stream_get_contents($pipes[1]);
    proc_close($proc);
}

write_script($call, 1);
run($cache, $main, 'shifted');

write_script($call, 2);
echo "--- Function at another position ---\n";
run($cache, $main, 'dl');

echo "--- Function not loaded ---\n";
run($cache, $main, 'missing');

echo "--- User function of the same name ---\n";
run($cache, $main, 'user');
?>
--CLEAN--
<?php
$cache = __DIR__ . '/' . 'gh25296';
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
v1
Hello hi
--- Function at another position ---
v1
Hello hi
--- Function not loaded ---
v1
Error: Call to undefined function dl_test_test2()
--- User function of the same name ---
v1
User hi
