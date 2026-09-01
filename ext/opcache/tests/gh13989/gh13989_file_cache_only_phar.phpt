--TEST--
GH-13989: file_cache_only should serve scripts from PHAR when source inside PHAR is stripped
--SKIPIF--
<?php
if (!extension_loaded('phar')) die('skip phar extension not available');
@mkdir(__DIR__ . '/gh13989_cache5', 0777, true);
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
opcache.file_cache="{PWD}/gh13989_cache5"
opcache.file_cache_only=1
opcache.file_cache_consistency_checks=0
opcache.file_update_protection=0
opcache.validate_timestamps=0
opcache.validate_permission=0
opcache.validate_root=0
phar.readonly=0
--EXTENSIONS--
opcache
phar
--FILE--
<?php
$php = PHP_BINARY;
$pharFile = __DIR__ . '/gh13989_app.phar';
$cacheDir = __DIR__ . '/gh13989_cache5';

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
    '-dphar.readonly=1',
]);

// Step 1: Create PHAR with real source and warm cache
$p = new Phar($pharFile);
$p['index.php'] = '<?php echo "from phar cache\n"; require __DIR__ . "/lib.php";';
$p['lib.php'] = '<?php echo "lib from phar\n";';
$p->setStub('<?php __HALT_COMPILER(); ?>');
unset($p);

$pharScript = 'phar://' . $pharFile . '/index.php';
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($pharScript) . " 2>&1");
echo "Step 1: " . trim($out) . "\n";

// Step 2: Replace PHAR with stripped source (no real code inside)
$p = new Phar($pharFile);
$p['index.php'] = '<?php /* stripped */ ?>';
$p['lib.php'] = '<?php /* stripped */ ?>';
$p->setStub('<?php __HALT_COMPILER(); ?>');
unset($p);

// Step 3: Run again — should serve original compiled output from file cache
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($pharScript) . " 2>&1");
echo "Step 2: " . trim($out) . "\n";

// Step 4: Run a third time to ensure cache was not invalidated
$out = shell_exec("$php $opcacheArgs " . escapeshellarg($pharScript) . " 2>&1");
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
removeDirRecursive(__DIR__ . '/gh13989_cache5');
@unlink(__DIR__ . '/gh13989_app.phar');
?>
--EXPECT--
Step 1: from phar cache
lib from phar
Step 2: from phar cache
lib from phar
Step 3: from phar cache
lib from phar
done
