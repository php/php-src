--TEST--
Bug #69477 (ZipArchive::extractTo() truncates path segments ending with dot)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
include __DIR__ . '/utils.inc';
$dir = __DIR__ . '/bug69477';
if (file_exists($dir)) rmdir_rf($dir);
mkdir($dir);
$zipfile = $dir . '/abc.zip';

$archive = new ZipArchive();

if (!$archive->open($zipfile, ZipArchive::CREATE)) {
    exit('failed: unable to create archive');
}

// (string) Entry path in the ZIP => (string) Expected actual target path
if (PHP_OS_FAMILY === 'Windows') {
    $paths = [
        '.a/b/c/file01.txt' => '.a/b/c/file01.txt',
        'a./b/c/file02.txt' => 'a_/b/c/file02.txt',
        'a/.b/c/file03.txt' => 'a/.b/c/file03.txt',
        'a/b./c/file04.txt' => 'a/b_/c/file04.txt',
        'a/b../c/file05.txt' => 'a/b._/c/file05.txt',
        'a/b.../c/file06.txt' => 'a/b.._/c/file06.txt',
        'a/..b/c/file07.txt' => 'a/..b/c/file07.txt',
        'a/...b/c/file08.txt' => 'a/...b/c/file08.txt',
        'a/../b./c./file09.txt' => 'b_/c_/file09.txt',
        '//../b./c./file10.txt' => 'b_/c_/file10.txt',
        '/../b./c./file11.txt' => 'b_/c_/file11.txt',
        'C:/a./b./file12.txt' => 'a_/b_/file12.txt',
        'a/b:/c/file13.txt' => 'c/file13.txt',
        'a/b/c/file14.' => 'a/b/c/file14_',
    ];
} else {
    $paths = [
        '.a/b/c/file01.txt' => '.a/b/c/file01.txt',
        'a./b/c/file02.txt' => 'a./b/c/file02.txt',
        'a/.b/c/file03.txt' => 'a/.b/c/file03.txt',
        'a/b./c/file04.txt' => 'a/b./c/file04.txt',
        'a/b../c/file05.txt' => 'a/b../c/file05.txt',
        'a/b.../c/file06.txt' => 'a/b.../c/file06.txt',
        'a/..b/c/file07.txt' => 'a/..b/c/file07.txt',
        'a/...b/c/file08.txt' => 'a/...b/c/file08.txt',
        'a/../b./c./file09.txt' => 'b./c./file09.txt',
        '//../b./c./file10.txt' => 'b./c./file10.txt',
        '/../b./c./file11.txt' => 'b./c./file11.txt',
        'C:/a./b./file12.txt' => 'a./b./file12.txt',
        'a/b:/c/file13.txt' => 'c/file13.txt',
        'a/b/c/file14.' => 'a/b/c/file14.',
    ];
}

foreach ($paths as $zippath => $realpath) {
    $archive->addFromString($zippath, $zippath . ' => ' . $realpath);
}

$archive->close();

$archive2 = new ZipArchive();

if (!$archive2->open($zipfile)) {
    exit('failed: unable to open archive2');
}

$archive2->extractTo($dir);
$archive2->close();

foreach ($paths as $zippath => $realpath) {
    if (!is_readable($dir . '/' . $realpath) || file_get_contents($dir . '/' . $realpath) !== $zippath . ' => ' . $realpath) {
        exit('failed: ' . $zippath);
    }
}

echo 'ok';
?>
--CLEAN--
<?php
include __DIR__ . '/utils.inc';
$dir = __DIR__ . '/bug69477';
rmdir_rf($dir);
?>
--EXPECT--
ok
