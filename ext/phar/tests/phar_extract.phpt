--TEST--
Phar: Phar::extractTo()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--CONFLICTS--
tempmanifest1.phar.php
--INI--
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/tempmanifest1.phar.php';
$pname = 'phar://' . $fname;

$a = new Phar($fname);
$a['file1.txt'] = 'hi';
$a['file2.txt'] = 'hi2';
$a['subdir/ectory/file.txt'] = 'hi3';
$a->mount($pname . '/mount', __FILE__);
$a->addEmptyDir('one/level');

$a->extractTo(__DIR__ . '/extract', 'mount');
$a->extractTo(__DIR__ . '/extract');

$out = array();

foreach (new RecursiveIteratorIterator(new RecursiveDirectoryIterator(__DIR__ . '/extract', 0x00003000), RecursiveIteratorIterator::CHILD_FIRST) as $p => $b) {
    $out[] = $p;
}

sort($out);

foreach ($out as $b) {
    echo "$b\n";
}

$a->extractTo(__DIR__ . '/extract1', 'file1.txt');
var_dump(file_get_contents(__DIR__ . '/extract1/file1.txt'));

$a->extractTo(__DIR__ . '/extract1', 'subdir/ectory/file.txt');
var_dump(file_get_contents(__DIR__ . '/extract1/subdir/ectory/file.txt'));

$a->extractTo(__DIR__ . '/extract1-2', array('file2.txt', 'one/level'));
var_dump(file_get_contents(__DIR__ . '/extract1-2/file2.txt'));
var_dump(is_dir(__DIR__ . '/extract1-2/one/level'));

try {
    $a->extractTo(__DIR__ . '/whatever', new stdClass());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $a->extractTo(array());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $a->extractTo('');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

file_put_contents(__DIR__ . '/oops', 'I is file');

try {
    $a->extractTo(__DIR__ . '/oops', 'file1.txt');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    $a->extractTo(__DIR__ . '/oops1', array(array(), 'file1.txt'));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    $a->extractTo(__DIR__ . '/extract', 'file1.txt');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

file_put_contents(__DIR__ . '/extract/file1.txt', 'first');
var_dump(file_get_contents(__DIR__ . '/extract/file1.txt'));

$a->extractTo(__DIR__ . '/extract', 'file1.txt', true);
var_dump(file_get_contents(__DIR__ . '/extract/file1.txt'));

try {
    $a->extractTo(str_repeat('a', 20000), 'file1.txt');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

$a[str_repeat('a', 20000)] = 'long';

try {
    $a->extractTo(__DIR__ . '/extract', str_repeat('a', 20000));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--CLEAN--
<?php
@rmdir(__DIR__ . '/whatever');
@unlink(__DIR__ . '/oops');
@rmdir(__DIR__ . '/oops1');
@unlink(__DIR__ . '/tempmanifest1.phar.php');
$e = __DIR__ . '/extract/';
@unlink($e . 'file1.txt');
@unlink($e . 'file2.txt');
@unlink($e . 'subdir/ectory/file.txt');
@rmdir($e . 'subdir/ectory');
@rmdir($e . 'subdir');
@rmdir($e . 'one/level');
@rmdir($e . 'one');
@rmdir($e);
$e = __DIR__ . '/extract1/';
@unlink($e . 'file1.txt');
@unlink($e . 'subdir/ectory/file.txt');
@rmdir($e . 'subdir/ectory');
@rmdir($e . 'subdir');
@rmdir($e);
$e = __DIR__ . '/extract1-2/';
@unlink($e . 'file2.txt');
@rmdir($e . 'one/level');
@rmdir($e . 'one');
@rmdir($e);
?>
--EXPECTF--
%sextract%cfile1.txt
%sextract%cfile2.txt
%sextract%cone
%sextract%csubdir
%sextract%csubdir%cectory
%sextract%csubdir%cectory%cfile.txt
string(2) "hi"
string(3) "hi3"
string(3) "hi2"
bool(false)
Phar::extractTo(): Argument #2 ($files) must be of type array|string|null, stdClass given
Phar::extractTo(): Argument #1 ($directory) must be of type string, array given
Invalid argument, extraction path must be non-zero length
Unable to use path "%soops" for extraction, it is a file, must be a directory
Invalid argument, array of filenames to extract contains non-string value
Extraction from phar "%stempmanifest1.phar.php" failed: Cannot extract "file1.txt" to "%sextract/file1.txt", path already exists
string(5) "first"
string(2) "hi"
Cannot extract to "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa...", destination directory is too long for filesystem
Extraction from phar "%stempmanifest1.phar.php" failed: Cannot extract "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..." to "%s...", extracted filename is too long for filesystem
