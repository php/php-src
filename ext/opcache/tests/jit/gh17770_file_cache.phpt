--TEST--
GH-17770: JIT scripts survive a file cache round trip
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open is unavailable');
$extension = ini_get('extension_dir') . DIRECTORY_SEPARATOR
    . (PHP_OS_FAMILY === 'Windows' ? 'php_opcache.dll' : 'opcache.so');
$cmd = [PHP_BINARY, '-n'];
if (file_exists($extension)) {
    array_push($cmd, '-d', 'zend_extension="' . $extension . '"');
}
array_push($cmd, '-d', 'opcache.enable_cli=1', '-d', 'opcache.jit_buffer_size=16M',
    '-d', 'opcache.jit=tracing', '-r', 'echo (int) (opcache_get_status(false)["jit"]["on"] ?? false);');
$process = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
$output = stream_get_contents($pipes[1]);
fclose($pipes[1]);
fclose($pipes[2]);
if (proc_close($process) !== 0 || $output !== '1') die('skip JIT is unavailable');
?>
--FILE--
<?php
$dir = __DIR__ . '/gh17770_file_cache';
$script = $dir . '/script.php';
@mkdir($dir);

$source = <<<'PHP'
<?php
function gh17770_value(int $value): int {
    return $value + 2;
}
function gh17770_array($values): int {
    $values[0] = 42;
    return $values[0];
}
class Gh17770Box {
    public function value(): int {
        return gh17770_value(40);
    }
}
$box = new Gh17770Box;
for ($i = 0; $i < 1000; $i++) {
    $box->value();
}
echo $box->value(), "\n";
echo gh17770_array([]), "\n";
PHP;

function runScript(string $script, string $cache, string $jit, bool $cacheOnly): void {
    $extension = ini_get('extension_dir') . DIRECTORY_SEPARATOR
        . (PHP_OS_FAMILY === 'Windows' ? 'php_opcache.dll' : 'opcache.so');
    $cmd = [PHP_BINARY, '-n'];
    if (file_exists($extension)) {
        array_push($cmd, '-d', 'zend_extension="' . $extension . '"');
    }
    array_push($cmd,
        '-d', 'opcache.enable_cli=1',
        '-d', 'opcache.jit_buffer_size=16M',
        '-d', 'opcache.jit=' . $jit,
        '-d', 'opcache.jit_hot_func=1',
        '-d', 'opcache.file_cache="' . $cache . '"',
        '-d', 'opcache.file_cache_only=' . (int) $cacheOnly,
        '-d', 'opcache.validate_timestamps=0',
        '-d', 'opcache.file_update_protection=0',
        '-r', '$before = opcache_get_status(false)["jit"]["buffer_free"] ?? 0; '
            . 'require $argv[1]; '
            . '$after = opcache_get_status(false)["jit"]["buffer_free"] ?? 0; '
            . 'echo (int) ($before > $after), "\\n";',
        $script,
    );
    $process = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    $output = stream_get_contents($pipes[1]);
    $errors = stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    $status = proc_close($process);
    if ($status !== 0 || $output !== ($cacheOnly ? "42\n42\n0\n" : "42\n42\n1\n") || $errors !== '') {
        throw new RuntimeException("JIT $jit, file_cache_only=" . (int) $cacheOnly
            . ": exit=$status, output=" . var_export($output, true) . ", errors=$errors");
    }
}

foreach (['1205', '1215', '1235', '1255'] as $jit) {
    $cache = $dir . '/' . $jit;
    @mkdir($cache);
    file_put_contents($script, $source);
    runScript($script, $cache, $jit, false);

    $files = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($cache));
    $found = false;
    foreach ($files as $file) {
        if ($file->isFile() && $file->getExtension() === 'bin') {
            $found = true;
            break;
        }
    }
    if (!$found) {
        throw new RuntimeException("JIT $jit did not write a file cache entry");
    }

    file_put_contents($script, '<?php echo "stale source\\n";');
    runScript($script, $cache, $jit, true);
    runScript($script, $cache, $jit, false);
    echo "$jit OK\n";
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh17770_file_cache';
if (is_dir($dir)) {
    $entries = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, FilesystemIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST,
    );
    foreach ($entries as $entry) {
        $entry->isDir() ? rmdir($entry->getPathname()) : unlink($entry->getPathname());
    }
    rmdir($dir);
}
?>
--EXPECT--
1205 OK
1215 OK
1235 OK
1255 OK
