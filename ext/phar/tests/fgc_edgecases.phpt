--TEST--
Phar: test edge cases of file_get_contents() function interception
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--INI--
phar.readonly=0
--FILE--
<?php

Phar::interceptFileFuncs();

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

file_get_contents(array());
chdir(__DIR__);
file_put_contents($fname, "blah\n");
file_put_contents("fgc_edgecases.txt", "test\n");
echo file_get_contents($fname);
unlink($fname);
mkdir($pname . '/oops');

file_put_contents($pname . '/foo/hi', '<?php
echo file_get_contents("foo/" . basename(__FILE__));
$context = stream_context_create();
file_get_contents("./hi", 0, $context, 0, -1);
echo file_get_contents("fgc_edgecases.txt");
set_include_path("' . addslashes(__DIR__) . '");
echo file_get_contents("fgc_edgecases.txt", true);
echo file_get_contents("./hi", 0, $context);
echo file_get_contents("../oops");
echo file_get_contents("./hi", 0, $context, 50000);
echo file_get_contents("./hi");
echo file_get_contents("./hi", 0, $context, 0, 0);
?>
');

include $pname . '/foo/hi';

?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(__DIR__ . '/fgc_edgecases.txt'); ?>
--EXPECTF--
Warning: file_get_contents() expects parameter 1 to be a valid path, array given in %sfgc_edgecases.php on line %d
blah
<?php
echo file_get_contents("foo/" . basename(__FILE__));
$context = stream_context_create();
file_get_contents("./hi", 0, $context, 0, -1);
echo file_get_contents("fgc_edgecases.txt");
set_include_path("%stests");
echo file_get_contents("fgc_edgecases.txt", true);
echo file_get_contents("./hi", 0, $context);
echo file_get_contents("../oops");
echo file_get_contents("./hi", 0, $context, 50000);
echo file_get_contents("./hi");
echo file_get_contents("./hi", 0, $context, 0, 0);
?>

Warning: file_get_contents(): length must be greater than or equal to zero in phar://%sfgc_edgecases.phar.php/foo/hi on line %d
test
test
<?php
echo file_get_contents("foo/" . basename(__FILE__));
$context = stream_context_create();
file_get_contents("./hi", 0, $context, 0, -1);
echo file_get_contents("fgc_edgecases.txt");
set_include_path("%stests");
echo file_get_contents("fgc_edgecases.txt", true);
echo file_get_contents("./hi", 0, $context);
echo file_get_contents("../oops");
echo file_get_contents("./hi", 0, $context, 50000);
echo file_get_contents("./hi");
echo file_get_contents("./hi", 0, $context, 0, 0);
?>

Warning: file_get_contents(phar://%sfgc_edgecases.phar.php/oops): failed to open stream: phar error: path "oops" is a directory in phar://%sfgc_edgecases.phar.php/foo/hi on line %d

Warning: file_get_contents(): Failed to seek to position 50000 in the stream in phar://%sfgc_edgecases.phar.php/foo/hi on line %d
<?php
echo file_get_contents("foo/" . basename(__FILE__));
$context = stream_context_create();
file_get_contents("./hi", 0, $context, 0, -1);
echo file_get_contents("fgc_edgecases.txt");
set_include_path("%stests");
echo file_get_contents("fgc_edgecases.txt", true);
echo file_get_contents("./hi", 0, $context);
echo file_get_contents("../oops");
echo file_get_contents("./hi", 0, $context, 50000);
echo file_get_contents("./hi");
echo file_get_contents("./hi", 0, $context, 0, 0);
?>
===DONE===
