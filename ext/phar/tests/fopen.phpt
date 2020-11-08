--TEST--
Phar: test fopen() interception
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.require_hash=1
phar.readonly=0
--FILE--
<?php
Phar::interceptFileFuncs();
$a = fopen(__FILE__, 'rb'); // this satisfies 1 line of code coverage
fclose($a);

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$a = new Phar($fname);
$a['index.php'] = '<?php
$a = fopen("dir/file1.txt", "r");
echo fread($a, 2);
fclose($a);
$a = fopen("file1.txt", "r", true);
echo fread($a, 2);
fclose($a);
$a = fopen("notfound.txt", "r", true);
?>';
$a['dir/file1.txt'] = 'hi';
$a['dir/file2.txt'] = 'hi2';
$a['dir/file3.txt'] = 'hi3';
$a->setStub('<?php
set_include_path("phar://" . __FILE__ . "/dir" . PATH_SEPARATOR . "phar://" . __FILE__);
include "index.php";
__HALT_COMPILER();');
include $fname;
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
hihi
Warning: fopen(notfound.txt): Failed to open stream: No such file or directory in phar://%sfopen.phar.php/index.php on line %d
