--TEST--
SPL: RecursiveDirectoryIterator::hasChildren() follow symlinks test
--FILE--
<?php

$dir = __DIR__ . DIRECTORY_SEPARATOR . 'symlinktest';

if (!mkdir($dir)) {
    die('Failed to create temporary directory for testing');
} elseif (!symlink(__DIR__, $dir . DIRECTORY_SEPARATOR . 'symlink')) {
    die('Failed to create symbolic link');
}

$it = new RecursiveDirectoryIterator($dir, FilesystemIterator::SKIP_DOTS | FilesystemIterator::FOLLOW_SYMLINKS | FilesystemIterator::KEY_AS_FILENAME);

var_dump($it->key());
var_dump($it->hasChildren());

$it->setFlags(FilesystemIterator::SKIP_DOTS | FilesystemIterator::KEY_AS_FILENAME);

var_dump($it->key());
var_dump($it->hasChildren());

?>
--EXPECT--
string(7) "symlink"
bool(true)
string(7) "symlink"
bool(false)
--CLEAN--
<?php
$dir = __DIR__ . DIRECTORY_SEPARATOR . 'symlinktest';
if (PHP_OS_FAMILY == 'Windows') {
    rmdir($dir . DIRECTORY_SEPARATOR . 'symlink');
} else {
    unlink($dir . DIRECTORY_SEPARATOR . 'symlink');
}
rmdir($dir);
?>
