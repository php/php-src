--TEST--
Phar: Phar::running()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);
$phar['index.php'] = '<?php
Phar::running(array());
var_dump(Phar::running());
var_dump(Phar::running(false));
?>';
include $pname . '/index.php';
var_dump(Phar::running());
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: Phar::running() expects parameter 1 to be bool, array given in phar://%sphar_running.phar.php/index.php on line 2
string(%d) "phar://%sphar_running.phar.php"
string(%d) "%sphar_running.phar.php"
string(0) ""
===DONE===
