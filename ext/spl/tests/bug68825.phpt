--TEST--
Bug #68825 (Exception in DirectoryIterator::getLinkTarget())
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    include_once __DIR__ . '/../../standard/tests/file/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
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
