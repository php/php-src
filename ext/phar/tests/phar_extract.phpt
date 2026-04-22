--TEST--
Phar: Phar::extractTo()
--EXTENSIONS--
phar
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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $a->extractTo(array());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $a->extractTo('');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

file_put_contents(__DIR__ . '/oops', 'I is file');

try {
    $a->extractTo(__DIR__ . '/oops', 'file1.txt');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $a->extractTo(__DIR__ . '/oops1', array(array(), 'file1.txt'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    $a->extractTo(__DIR__ . '/extract', 'file1.txt');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

file_put_contents(__DIR__ . '/extract/file1.txt', 'first');
var_dump(file_get_contents(__DIR__ . '/extract/file1.txt'));

$a->extractTo(__DIR__ . '/extract', 'file1.txt', true);
var_dump(file_get_contents(__DIR__ . '/extract/file1.txt'));

try {
    $a->extractTo(str_repeat('a', 20000), 'file1.txt');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

$a[str_repeat('a', 20000)] = 'long';

try {
    $a->extractTo(__DIR__ . '/extract', str_repeat('a', 20000));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

?>
--CLEAN--
<?php
unlink(__DIR__ . '/oops');
rmdir(__DIR__ . '/oops1');
unlink(__DIR__ . '/tempmanifest1.phar.php');
$e = __DIR__ . '/extract/';
unlink($e . 'file1.txt');
unlink($e . 'file2.txt');
unlink($e . 'subdir/ectory/file.txt');
rmdir($e . 'subdir/ectory');
rmdir($e . 'subdir');
rmdir($e . 'one');
rmdir($e);
$e = __DIR__ . '/extract1/';
unlink($e . 'file1.txt');
unlink($e . 'subdir/ectory/file.txt');
rmdir($e . 'subdir/ectory');
rmdir($e . 'subdir');
rmdir($e);
$e = __DIR__ . '/extract1-2/';
unlink($e . 'file2.txt');
rmdir($e . 'one');
rmdir($e);
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
TypeError: Phar::extractTo(): Argument #2 ($files) must be of type array|string|null, stdClass given
TypeError: Phar::extractTo(): Argument #1 ($directory) must be of type string, array given
ValueError: Phar::extractTo(): Argument #1 ($directory) must not be empty
RuntimeException: Unable to use path "%soops" for extraction, it is a file, must be a directory
TypeError: Phar::extractTo(): Argument #2 ($files) must be an array of strings, array given
PharException: Extraction from phar "%stempmanifest1.phar.php" failed: Cannot extract "file1.txt" to "%sextract%cfile1.txt", path already exists
string(5) "first"
string(2) "hi"
ValueError: Phar::extractTo(): Argument #1 ($directory) must be less than %d bytes
PharException: Extraction from phar "%stempmanifest1.phar.php" failed: Cannot extract "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..." to "%s...", extracted filename is too long for filesystem
