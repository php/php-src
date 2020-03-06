--TEST--
Bug #68825 (Exception in DirectoryIterator::getLinkTarget())
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    $fn = "bug68825.lnk";
    $ret = exec("mklink $fn " . __FILE__ .' 2>&1', $out);
    @unlink($fn);
    if (strpos($ret, 'privilege')) {
        die('skip. SeCreateSymbolicLinkPrivilege not enable for this user.');
    }
}
?>
--FILE--
<?php
$dir = __DIR__ . '/bug68825';

if (!mkdir($dir)) {
    die('Failed to create temporary directory for testing');
} else if (!symlink(__FILE__, $dir . '/bug')) {
    die('Failed to create symbolic link');
}

$di = new \DirectoryIterator($dir);
foreach ($di as $entry) {
    if ('bug' === $entry->getFilename()) {
        var_dump($entry->getLinkTarget());
    }
}
?>
===DONE===
--EXPECTF--
string(%d) "%s%eext%espl%etests%ebug68825.php"
===DONE===
--CLEAN--
<?php
$dir = __DIR__ . '/bug68825';
unlink($dir . '/bug');
rmdir($dir);
?>
