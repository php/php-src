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
mkdir($dir);
symlink(__FILE__, "$dir/foo");

$di = new \DirectoryIterator($dir);
foreach ($di as $entry) {
    if ('foo' === $entry->getFilename()) {
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
unlink("$dir/foo");
rmdir($dir);
?>
