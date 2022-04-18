--TEST--
Phar: test edge cases of intercepted functions when the underlying phar archive has been unlinkArchive()d
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (defined('PHP_WINDOWS_VERSION_MAJOR')) die("skip");
?>
--INI--
phar.readonly=0
--FILE--
<?php
Phar::interceptFileFuncs();
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.2.php';
$pname = 'phar://' . $fname;
file_put_contents($fname2, '<?php Phar::unlinkArchive("' . addslashes($fname) . '");');
file_put_contents($pname . '/foo/hi', '<?php
include "' . addslashes($fname2) . '";
readfile("foo/hi");
fopen("foo/hi", "r");
echo file_get_contents("foo/hi");
var_dump(is_file("foo/hi"),is_link("foo/hi"),is_dir("foo/hi"),file_exists("foo/hi"),stat("foo/hi"));
opendir("foo/hi");
?>
');
include $pname . '/foo/hi';
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.2.php');
?>
--EXPECTF--
Warning: readfile(foo/hi): Failed to open stream: No such file or directory in phar://%sphar_gobyebye.phar.php/foo/hi on line %d

Warning: fopen(foo/hi): Failed to open stream: No such file or directory in phar://%sphar_gobyebye.phar.php/foo/hi on line %d

Warning: file_get_contents(foo/hi): Failed to open stream: No such file or directory in phar://%sphar_gobyebye.phar.php/foo/hi on line %d

Warning: stat(): stat failed for foo/hi in phar://%sphar_gobyebye.phar.php/foo/hi on line %d
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: opendir(foo/hi): Failed to open directory: No such file or directory in phar://%sphar_gobyebye.phar.php/foo/hi on line %d
