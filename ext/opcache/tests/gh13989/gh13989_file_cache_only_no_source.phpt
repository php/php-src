--TEST--
GH-13989: file_cache_only should serve scripts without source files on disk
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh13989_cache', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh13989_cache"
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
// Create a temporary PHP file
$file = __DIR__ . '/gh13989_test_script.php';
file_put_contents($file, '<?php echo "cached output\n";');

// Compile it to populate file cache
opcache_compile_file($file);

// Include it once to verify it works
include $file;

// Now remove the source file
unlink($file);

// Include again — should serve from file cache without source
include $file;

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
removeDirRecursive(__DIR__ . '/gh13989_cache');
@unlink(__DIR__ . '/gh13989_test_script.php');
?>
--EXPECT--
cached output
cached output
done
