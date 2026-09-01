--TEST--
GH-13989: file_cache_only should resolve relative paths with . and .. when source is missing
--SKIPIF--
<?php
@mkdir(__DIR__ . '/gh13989_cache7', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh13989_cache7"
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
$php = PHP_BINARY;
$baseDir = __DIR__ . '/gh13989_relpath';
$subDir = $baseDir . '/sub';
$cacheDir = __DIR__ . '/gh13989_cache7';

@mkdir($subDir, 0777, true);

// Create a script in the base directory
file_put_contents($baseDir . '/target.php', '<?php echo "from relative path\n";');

$opcacheArgs = implode(' ', [
    '-dopcache.enable=1',
    '-dopcache.enable_cli=1',
    '-dopcache.jit=disable',
    '-dopcache.file_cache=' . escapeshellarg($cacheDir),
    '-dopcache.file_cache_only=1',
    '-dopcache.file_cache_consistency_checks=0',
    '-dopcache.file_update_protection=0',
    '-dopcache.validate_timestamps=0',
    '-dopcache.validate_permission=0',
    '-dopcache.validate_root=0',
]);

// Step 1: Warm cache using a path with ./
$script1 = $baseDir . '/./target.php';
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($script1) . " 2>&1");
echo "Step 1 (./): " . trim($out) . "\n";

// Step 2: Warm cache using a path with sub/../
$script2 = $baseDir . '/sub/../target.php';
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($script2) . " 2>&1");
echo "Step 2 (sub/../): " . trim($out) . "\n";

// Step 3: Remove source file
unlink($baseDir . '/target.php');

// Step 4: Run with ./ path — should serve from cache
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($script1) . " 2>&1");
echo "Step 3 (./ no source): " . trim($out) . "\n";

// Step 5: Run with sub/../ path — should serve from same cache entry
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($script2) . " 2>&1");
echo "Step 4 (sub/../ no source): " . trim($out) . "\n";

// Step 6: Run with canonical absolute path — should serve from same cache entry
$script3 = $baseDir . '/target.php';
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($script3) . " 2>&1");
echo "Step 5 (absolute no source): " . trim($out) . "\n";

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
removeDirRecursive(__DIR__ . '/gh13989_cache7');
removeDirRecursive(__DIR__ . '/gh13989_relpath');
?>
--EXPECT--
Step 1 (./): from relative path
Step 2 (sub/../): from relative path
Step 3 (./ no source): from relative path
Step 4 (sub/../ no source): from relative path
Step 5 (absolute no source): from relative path
done
