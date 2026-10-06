--TEST--
GH-13989: file_cache_only should serve included files without source on disk
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh13989_cache2', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh13989_cache2"
opcache.file_cache_only=1
opcache.file_cache_consistency_checks=0
opcache.file_update_protection=0
opcache.validate_timestamps=0
opcache.validate_permission=0
opcache.validate_root=0
--EXTENSIONS--
opcache
--FILE--
<?php
$lib = __DIR__ . '/gh13989_lib.php';
file_put_contents($lib, '<?php echo "from lib\n";');

// Include to populate cache
include $lib;

// Remove source
unlink($lib);

// Include again without source — should serve from file cache
include $lib;

echo "done\n";
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
removeDirRecursive(__DIR__ . '/gh13989_cache2');
@unlink(__DIR__ . '/gh13989_lib.php');
?>
--EXPECT--
from lib
from lib
done
