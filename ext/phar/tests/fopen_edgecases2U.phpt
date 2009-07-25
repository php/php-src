--TEST--
Phar: test edge cases of fopen() function interception #2 (PHP 6)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (version_compare(PHP_VERSION, "6.0.0-dev", "<")) die("skip Unicode support required"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
Phar::interceptFileFuncs();
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

fopen(array(), 'r');
chdir(dirname(__FILE__));
file_put_contents($fname, b"blah\n");
file_put_contents("foob", b"test\n");
$a = fopen($fname, 'rb');
echo fread($a, 1000);
fclose($a);
unlink($fname);
mkdir($pname . '/oops');
file_put_contents($pname . '/foo/hi', b'<?php
$context = stream_context_create();
$a = fopen("foob", "rb", false, $context);
echo fread($a, 1000);
fclose($a);
fopen("../oops", "r");
?>
');
include $pname . '/foo/hi';
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php rmdir(dirname(__FILE__) . '/poo'); ?>
<?php unlink(dirname(__FILE__) . '/foob'); ?>
--EXPECTF--
Notice: Array to string conversion in %sfopen_edgecases2U.php on line 6

Warning: fopen(Array): failed to open stream: No such file or directory in %sfopen_edgecases2U.php on line 6
blah
test

Warning: fopen(phar://%sfopen_edgecases2U.phar.php/oops): failed to open stream: phar error: path "oops" is a directory in phar://%sfopen_edgecases2U.phar.php/foo/hi on line 6
===DONE===