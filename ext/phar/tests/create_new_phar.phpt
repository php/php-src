--TEST--
Phar: create a completely new phar
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

file_put_contents('phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php/a.php',
    "brand new!\n");
include 'phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php/a.php';

$fileName = "ChineseFile\xE5\x84\xB7\xE9\xBB\x91.php";
file_put_contents('phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php/$fileName.php',
    'Text in utf8 file.');
include 'phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php/$fileName.php';
?>

--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
brand new!
Text in utf8 file.
