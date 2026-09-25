--TEST--
GH-13989: file_cache_only with validate_timestamps=1 should serve cached scripts when source is deleted
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh13989_cache4', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh13989_cache4"
opcache.file_cache_only=1
opcache.file_cache_consistency_checks=0
opcache.file_update_protection=0
opcache.validate_timestamps=1
opcache.validate_permission=0
opcache.validate_root=0
--EXTENSIONS--
opcache
--FILE--
<?php
$php = PHP_BINARY;
$script = __DIR__ . '/gh13989_validate_ts_test.php';
$cacheDir = __DIR__ . '/gh13989_cache4';

$opcacheArgs = implode(' ', [
    '-dopcache.enable=1',
    '-dopcache.enable_cli=1',
    '-dopcache.jit=disable',
    '-dopcache.file_cache=' . escapeshellarg($cacheDir),
    '-dopcache.file_cache_only=1',
    '-dopcache.file_cache_consistency_checks=0',
    '-dopcache.file_update_protection=0',
    '-dopcache.validate_timestamps=1',
    '-dopcache.validate_permission=0',
    '-dopcache.validate_root=0',
]);

// Step 1: Create script and run to populate cache
file_put_contents($script, '<?php echo "cached with timestamps\n";');
$out = shell_exec("$php $opcacheArgs $script 2>&1");
echo "Step 1: " . trim($out) . "\n";

// Step 2: Remove source and run again — should serve from cache
// even with validate_timestamps=1, because source is missing
unlink($script);
$out = shell_exec("$php $opcacheArgs $script 2>&1");
echo "Step 2: " . trim($out) . "\n";

// Step 3: Run a third time to ensure cache file was not deleted
$out = shell_exec("$php $opcacheArgs $script 2>&1");
echo "Step 3: " . trim($out) . "\n";

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
removeDirRecursive(__DIR__ . '/gh13989_cache4');
@unlink(__DIR__ . '/gh13989_validate_ts_test.php');
?>
--EXPECT--
Step 1: cached with timestamps
Step 2: cached with timestamps
Step 3: cached with timestamps
done
