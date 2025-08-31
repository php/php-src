--TEST--
Phar: include_path with phar:// wrapper
--EXTENSIONS--
phar
--CONFLICTS--
tempmanifest1.phar.php
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/tempmanifest1.phar.php';
$a = new Phar($fname);
$a['file1.php'] = 'file1.php
';
$a['test/file1.php'] = 'test/file1.php
';
unset($a);
set_include_path('.' . PATH_SEPARATOR . 'phar://' . $fname);
include 'file1.php';
set_include_path('.' . PATH_SEPARATOR . 'phar://' . $fname . '/test');
include 'file1.php';
include 'file2.php';
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/tempmanifest1.phar.php');
?>
--EXPECTF--
file1.php
test/file1.php

Warning: include(file2.php): Failed to open stream: No such file or directory in %sinclude_path.php on line %d

Warning: include(): Failed opening 'file2.php' for inclusion (include_path='%sphar://%stempmanifest1.phar.php/test') in %sinclude_path.php on line %d
