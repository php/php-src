--TEST--
File cache checksum is the Adler-32 of the cached script
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
@mkdir(__DIR__ . '/file_cache_checksum_cache', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/file_cache_checksum_cache"
opcache.file_cache_only=1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php

/* Spans several Adler-32 blocks of 5552 bytes, with every byte value and a
 * long run of 0xff, the worst case for the sums. */
$bytes = str_repeat(implode('', array_map('chr', range(0, 255))), 64) . str_repeat("\xff", 20011);
$file = __DIR__ . '/file_cache_checksum.inc';
file_put_contents($file, "<?php\nreturn '" . addcslashes($bytes, "\\'") . "';\n");
var_dump(opcache_compile_file($file));

$iterator = new RecursiveIteratorIterator(
    new RecursiveDirectoryIterator(__DIR__ . '/file_cache_checksum_cache', RecursiveDirectoryIterator::SKIP_DOTS)
);
foreach ($iterator as $fileinfo) {
    if (str_ends_with($fileinfo->getFilename(), 'file_cache_checksum.inc.bin')) {
        $bin = file_get_contents($fileinfo->getPathname());
    }
}

/* The header is zend_file_cache_metainfo (zend_file_cache.c): 80 bytes, with
 * mem_size and str_size at offset 40 and the checksum of the rest at 72. */
['mem' => $mem, 'str' => $str] = unpack('Qmem/Qstr', $bin, 40);
var_dump(strlen($bin) === 80 + $mem + $str && $mem + $str > 3 * 5552);
var_dump(sprintf('%08x', unpack('L', $bin, 72)[1]) === hash('adler32', substr($bin, 80)));

?>
--CLEAN--
<?php
function removeDirRecursive($dir) {
    if (!is_dir($dir)) return;
    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($iterator as $fileinfo) {
        if ($fileinfo->isDir()) {
            @rmdir($fileinfo->getRealPath());
        } else {
            @unlink($fileinfo->getRealPath());
        }
    }
    @rmdir($dir);
}
removeDirRecursive(__DIR__ . '/file_cache_checksum_cache');
@unlink(__DIR__ . '/file_cache_checksum.inc');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
