--TEST--
Bug #81211 (Symlinks are followed when creating PHAR archive)
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
--FILE--
<?php
mkdir(__DIR__ . '/bug81211');
mkdir(__DIR__ . '/bug81211/foobar');
mkdir(__DIR__ . '/bug81211/foo');

file_put_contents(__DIR__ . '/bug81211/foobar/file', 'this file should NOT be included in the archive!');
symlink(__DIR__ . '/bug81211/foobar/file', __DIR__ . '/bug81211/foo/symlink');

$archive = new PharData(__DIR__ . '/bug81211/archive.tar');
try {
    $archive->buildFromDirectory(__DIR__ . '/bug81211/foo');
} catch (UnexpectedValueException $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
try {
    $archive->buildFromIterator(new RecursiveDirectoryIterator(__DIR__ . '/bug81211/foo', FilesystemIterator::SKIP_DOTS), __DIR__ . '/bug81211/foo');
} catch (UnexpectedValueException $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/bug81211/archive.tar');
@unlink(__DIR__ . '/bug81211/foo/symlink');
@unlink(__DIR__ . '/bug81211/foobar/file');
@rmdir(__DIR__ . '/bug81211/foo');
@rmdir(__DIR__ . '/bug81211/foobar');
@rmdir(__DIR__ . '/bug81211');
?>
--EXPECTF--
Iterator RecursiveIteratorIterator returned a path "%s%ebug81211%efoobar%efile" that is not in the base directory "%s%ebug81211%efoo"
Iterator RecursiveDirectoryIterator returned a path "%s%ebug81211%efoobar%efile" that is not in the base directory "%s%ebug81211%efoo"
