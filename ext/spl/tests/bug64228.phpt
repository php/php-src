--TEST--
Bug #64228 (RecursiveDirectoryIterator always assumes SKIP_DOTS)
--FILE--
<?php
$dirs = array();
$empty_dir = __DIR__ . "/empty";
@mkdir($empty_dir);

$i = new RecursiveDirectoryIterator($empty_dir, FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO); // Note the absence of FilesystemIterator::SKIP_DOTS
foreach ($i as $key => $value) {
    $dirs[] = $value->getFileName();
}

@rmdir($empty_dir);

sort($dirs);
print_r($dirs);
?>
--EXPECT--
Array
(
    [0] => .
    [1] => ..
)
