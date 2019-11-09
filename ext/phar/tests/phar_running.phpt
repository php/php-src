--TEST--
Phar: Phar::running()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);
$phar['index.php'] = '<?php
var_dump(Phar::running());
var_dump(Phar::running(false));
?>';
include $pname . '/index.php';
var_dump(Phar::running());
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
string(%d) "phar://%sphar_running.phar.php"
string(%d) "%sphar_running.phar.php"
string(0) ""
