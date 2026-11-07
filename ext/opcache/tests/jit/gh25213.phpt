--TEST--
GH-25213 (JIT buffer misalignment crashes OPcache when shared memory fills up on Windows)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
if (ini_get('opcache.jit') === false) die('skip PHP is compiled without JIT');
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.memory_consumption=16
opcache.interned_strings_buffer=1
opcache.max_accelerated_files=20000
opcache.jit=tracing
opcache.jit_buffer_size=64M
--FILE--
<?php
if (!(opcache_get_status(false)['jit']['enabled'] ?? false)) {
    throw new RuntimeException('JIT is not enabled');
}
$dir = __DIR__ . '/gh25213_tmp';
mkdir($dir);
for ($i = 0; $i < 20000; $i++) {
    $file = "$dir/c$i.php";
    file_put_contents($file, "<?php class C$i extends ArrayObject { public function m(int \$a): int { return \$a + $i; } }\n");
    require $file;
    unlink($file);
    $status = opcache_get_status(false);
    if ($status['cache_full']) {
        break;
    }
}
var_dump($status['cache_full']);
var_dump($status['memory_usage']['free_memory'] < 65536);
var_dump($status['opcache_statistics']['num_cached_keys'] < $status['opcache_statistics']['max_cached_keys']);
rmdir($dir);
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh25213_tmp';
if (is_dir($dir)) {
    foreach (glob("$dir/*") as $file) {
        unlink($file);
    }
    rmdir($dir);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
