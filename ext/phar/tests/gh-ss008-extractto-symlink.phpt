--TEST--
extractTo: symlink traversal via pre-existing symlink in destination directory
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    if (false === include __DIR__ . '/../../standard/tests/file/windows_links/common.inc') {
        die('skip windows_links/common.inc is not available');
    }
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--INI--
phar.readonly=0
--FILE--
<?php
$pharFile = __DIR__ . '/gh_ss008.phar';
$dest     = __DIR__ . '/gh_ss008_dest';
$target   = __DIR__ . '/gh_ss008_target';

@mkdir($dest, 0777, true);
@mkdir($target, 0777, true);

$p = new Phar($pharFile);
$p->addFromString('subdir/evil.txt', 'should not arrive');
unset($p);

symlink($target, $dest . '/subdir');

try {
    $phar = new Phar($pharFile);
    $phar->extractTo($dest, null, true);
    echo "EXTRACTED (unexpected)\n";
} catch (PharException $e) {
    echo "Caught PharException: " . $e->getMessage() . "\n";
}

if (file_exists($target . '/evil.txt')) {
    echo "FAIL: target was written\n";
} else {
    echo "OK: target not written\n";
}
?>
--CLEAN--
<?php
$pharFile = __DIR__ . '/gh_ss008.phar';
$dest     = __DIR__ . '/gh_ss008_dest';
$target   = __DIR__ . '/gh_ss008_target';

@unlink($dest . '/subdir');
@unlink($target . '/evil.txt');
@rmdir($dest);
@rmdir($target);
@unlink($pharFile);
?>
--EXPECTF--
Caught PharException: %s
OK: target not written
