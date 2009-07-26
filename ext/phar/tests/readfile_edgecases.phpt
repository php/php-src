--TEST--
Phar: test edge cases of readfile() function interception
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
<?php if (version_compare(PHP_VERSION, "6.0", ">=")) die("skip requires php older than 6.0"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
Phar::interceptFileFuncs();
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

readfile(array());
chdir(dirname(__FILE__));
file_put_contents($fname, "blah\n");
file_put_contents("foob", "test\n");
readfile($fname);
unlink($fname);
mkdir($pname . '/oops');
file_put_contents($pname . '/foo/hi', '<?php
readfile("foo/" . basename(__FILE__));
$context = stream_context_create();
readfile("foob");
set_include_path("' . addslashes(dirname(__FILE__)) . '");
readfile("foob", true);
readfile("./hi", 0, $context);
readfile("../oops");
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
Warning: readfile() expects parameter 1 to be string, array given in %sreadfile_edgecases.php on line %d
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