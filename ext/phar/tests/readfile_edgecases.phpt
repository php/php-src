--TEST--
Phar: test edge cases of readfile() function interception
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.readonly=0
--FILE--
<?php
Phar::interceptFileFuncs();
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

chdir(__DIR__);
file_put_contents($fname, "blah\n");
file_put_contents("foob", "test\n");
readfile($fname);
unlink($fname);
mkdir($pname . '/oops');
file_put_contents($pname . '/foo/hi', '<?php
readfile("foo/" . basename(__FILE__));
$context = stream_context_create();
readfile("foob");
set_include_path("' . addslashes(__DIR__) . '");
readfile("foob", true);
readfile("./hi", 0, $context);
readfile("../oops");
?>
');
include $pname . '/foo/hi';
?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php rmdir(__DIR__ . '/poo'); ?>
<?php unlink(__DIR__ . '/foob'); ?>
--EXPECTF--
blah
<?php
readfile("foo/" . basename(__FILE__));
$context = stream_context_create();
readfile("foob");
set_include_path("%stests");
readfile("foob", true);
readfile("./hi", 0, $context);
readfile("../oops");
?>
test
test
<?php
readfile("foo/" . basename(__FILE__));
$context = stream_context_create();
readfile("foob");
set_include_path("%stests");
readfile("foob", true);
readfile("./hi", 0, $context);
readfile("../oops");
?>

Warning: readfile(phar://%sreadfile_edgecases.phar.php/oops): failed to open stream: phar error: path "oops" is a directory in phar://%sreadfile_edgecases.phar.php/foo/hi on line %d
===DONE===
