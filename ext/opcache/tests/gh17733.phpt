--TEST--
GH-17733 (System entropy does not take into account function order, resulting in wrong functions possibly called in different configurations)
--EXTENSIONS--
opcache
--FILE--
<?php
$cache = __DIR__ . '/' . 'gh17733';
$script = $cache . '/' . 'call.php';
mkdir($cache);

function write_script(string $script, int $version): void {
    file_put_contents($script, <<<PHP
    <?php
    echo "v$version\n";
    try {
        var_dump(str_repeat("ab", 3));
    } catch (\Throwable \$e) {
        echo \$e::class, ": ", \$e->getMessage(), "\n";
    }
    PHP);
}

function run(string $cache, string $script, string $disable_functions): void {
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
        '-d', 'disable_functions=' . $disable_functions,
        $script,
    );
    $proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['redirect', 1]], $pipes);
    echo stream_get_contents($pipes[1]);
    proc_close($proc);
}

function system_ids(string $cache): int {
    $pattern = PHP_OS_FAMILY === 'Windows' ? '/*/*' : '/*';
    return count(glob($cache . $pattern, GLOB_ONLYDIR));
}

write_script($script, 1);
run($cache, $script, '');
var_dump(system_ids($cache));

write_script($script, 2);
echo "--- Same environment reuses the cache ---\n";
run($cache, $script, '');
var_dump(system_ids($cache));

echo "--- Shifted function table ---\n";
run($cache, $script, 'zend_version');
var_dump(system_ids($cache));

echo "--- Disabled function ---\n";
run($cache, $script, 'str_repeat');
var_dump(system_ids($cache));

write_script($script, 3);
echo "--- Each environment reuses its own cache ---\n";
run($cache, $script, 'zend_version');
run($cache, $script, 'str_repeat');
run($cache, $script, '');
var_dump(system_ids($cache));
?>
--CLEAN--
<?php
$cache = __DIR__ . '/' . 'gh17733';
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
string(6) "ababab"
int(1)
--- Same environment reuses the cache ---
v1
string(6) "ababab"
int(1)
--- Shifted function table ---
v2
string(6) "ababab"
int(2)
--- Disabled function ---
v2
Error: Call to undefined function str_repeat()
int(3)
--- Each environment reuses its own cache ---
v2
string(6) "ababab"
v2
Error: Call to undefined function str_repeat()
v1
string(6) "ababab"
int(3)
