--TEST--
Phar: test fopen() interception
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
<?php if (substr(phpversion(), 0, 3) == '5.2') die("skip PHP >= 5.3 required for this test");?>
--INI--
phar.require_hash=1
phar.readonly=0
--FILE--
<?php
Phar::interceptFileFuncs();
$a = fopen(__FILE__, 'rb'); // this satisfies 1 line of code coverage
fclose($a);
$a = fopen(); // this satisfies another line of code coverage

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
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
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: fopen() expects at least 2 parameters, 0 given in %sfopen.php on line %d
hihi
Warning: fopen(notfound.txt): failed to open stream: No such file or directory in phar://%sfopen.phar.php/index.php on line %d
===DONE===
