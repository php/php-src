--TEST--
Bug #78863 (DirectoryIterator class silently truncates after a null byte)
--FILE--
<?php
$dir = __DIR__ . '/bug78863';
mkdir($dir);
touch("$dir/bad");
mkdir("$dir/sub");
touch("$dir/sub/good");

$it = new DirectoryIterator(__DIR__ . "/bug78863\0/sub");
foreach ($it as $fileinfo) {
    if (!$fileinfo->isDot()) {
        var_dump($fileinfo->getFilename());
    }
}
?>
--EXPECTF--
Fatal error: Uncaught UnexpectedValueException: DirectoryIterator::__construct() expects parameter 1 to be a valid path, string given in %s:%d
Stack trace:
#0 %s(%d): DirectoryIterator->__construct('%s')
#1 {main}
  thrown in %s on line %d
--CLEAN--
<?php
$dir = __DIR__ . '/bug78863';
unlink("$dir/sub/good");
rmdir("$dir/sub");
unlink("$dir/bad");
rmdir($dir);
?>
