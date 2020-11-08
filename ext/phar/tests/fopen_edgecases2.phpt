--TEST--
Phar: test edge cases of fopen() function interception #2
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
Phar::interceptFileFuncs();
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

try {
    fopen(array(), 'r');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
chdir(__DIR__);
file_put_contents($fname, "blah\n");
file_put_contents("fopen_edgecases2.txt", "test\n");
$a = fopen($fname, 'rb');
echo fread($a, 1000);
fclose($a);
unlink($fname);
mkdir($pname . '/oops');
file_put_contents($pname . '/foo/hi', '<?php
$context = stream_context_create();
$a = fopen("fopen_edgecases2.txt", "rb", false, $context);
echo fread($a, 1000);
fclose($a);
fopen("../oops", "r");
?>
');
include $pname . '/foo/hi';
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(__DIR__ . '/fopen_edgecases2.txt'); ?>
--EXPECTF--
fopen(): Argument #1 ($filename) must be of type string, array given
blah
test

Warning: fopen(phar://%sfopen_edgecases2.phar.php/oops): Failed to open stream: phar error: path "oops" is a directory in phar://%sfopen_edgecases2.phar.php/foo/hi on line %d
