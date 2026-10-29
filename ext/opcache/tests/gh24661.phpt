--TEST--
GH-24661 (Wrong function called or crash if the function table changed)
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
$cache = __DIR__ . DIRECTORY_SEPARATOR . 'gh24661';
$script = $cache . DIRECTORY_SEPARATOR . 'main.php';
mkdir($cache);
file_put_contents($script, <<<'PHP'
<?php
if (($argv[1] ?? '') === 'dl') {
    dl('dl_test');
} elseif (($argv[1] ?? '') === 'user') {
    include __DIR__ . '/user.php';
}
include __DIR__ . '/call.php';
PHP);
file_put_contents($cache . DIRECTORY_SEPARATOR . 'user.php', <<<'PHP'
<?php
function dl_test_test2(string $str = ''): string {
    return "User $str";
}
PHP);
/* call.php is compiled after dl(). The try/catch keeps the call in f() from being
 * specialized to ZEND_INIT_FCALL_OFFSET. Both calls are compiled for the internal
 * function and must not go to a user function of the same name. */
file_put_contents($cache . DIRECTORY_SEPARATOR . 'call.php', <<<'PHP'
<?php
set_exception_handler(function (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
});
function f() {
    try {
        return dl_test_test2('f');
    } catch (Error $e) {
        return $e::class . ": " . $e->getMessage();
    }
}
echo f(), "\n";
echo dl_test_test2('main'), "\n";
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

echo "--- Compiled with dl_test loaded ---\n";
run($cache, [$script, 'dl']);
echo "--- Cached, run without dl_test ---\n";
run($cache, [$script]);
echo "--- Cached, run with a user function of the same name ---\n";
run($cache, [$script, 'user']);
?>
--CLEAN--
<?php
$cache = __DIR__ . DIRECTORY_SEPARATOR . 'gh24661';
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
--- Compiled with dl_test loaded ---
Hello f
Hello main
--- Cached, run without dl_test ---
Error: Call to undefined function dl_test_test2()
Error: Call to undefined function dl_test_test2()
--- Cached, run with a user function of the same name ---
Error: Call to undefined function dl_test_test2()
Error: Call to undefined function dl_test_test2()
