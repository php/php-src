--TEST--
RecursiveDirectoryIterator with dir path long or of edge case length
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();

if (strlen(__DIR__) > 259) die("skip Unsuitable starting path length");
?>
--FILE--
<?php

$need_len = 1024;
//$need_len = 259;
$dir = __DIR__;
while ($need_len - strlen($dir) > 32) {
    $dir .= DIRECTORY_SEPARATOR . str_repeat("a", 32);
}
$dir .= DIRECTORY_SEPARATOR . str_repeat("a", $need_len - strlen($dir));
mkdir($dir, 0700, true);

$fl = $dir . DIRECTORY_SEPARATOR . "hello.txt";
file_put_contents($fl, "");


$start = substr($dir, 0, strpos($dir, DIRECTORY_SEPARATOR, strlen(__DIR__)+1));
$iter = new RecursiveIteratorIterator(
    new RecursiveDirectoryIterator(
        $start,
        FilesystemIterator::SKIP_DOTS
    ),
    RecursiveIteratorIterator::CHILD_FIRST
);

foreach ($iter as $item) {
    if (!$item->isDir()) {
        var_dump($item->getPathname());
    }
}

$iter->rewind();
foreach ($iter as $item) {
    if ($item->isDir()) {
        rmdir($item->getPathname());
    } else {
        unlink($item->getPathname());
    }
}
rmdir($start);
var_dump(file_exists($start));

/*unlink($fl);
do {
    rmdir($dir);
    $dir = dirname($dir);
} while (__DIR__ != $dir);*/

?>
--EXPECTF--
string(%d) "%shello.txt"
bool(false)
