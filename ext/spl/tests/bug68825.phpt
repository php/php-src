--TEST--
Bug #68825 (Exception in DirectoryIterator::getLinkTarget())
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
--EXPECTF--
string(%d) "%s%eext%espl%etests%ebug68825.php"
--CLEAN--
<?php
$dir = __DIR__ . '/bug68825';
unlink($dir . '/bug');
rmdir($dir);
?>
