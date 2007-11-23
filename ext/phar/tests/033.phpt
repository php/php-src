--TEST--
Phar::chmod
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.1.phar.php';
$pname = 'phar://hio';
$file = '<?php include "' . $pname . '/a.php"; __HALT_COMPILER(); ?>';

$files = array();
$files['a.php']   = '<?php echo "This is a\n"; include "'.$pname.'/b.php"; ?>';
include 'phar_test.inc';
$a = new Phar($fname);
var_dump($a['a.php']->isExecutable());
$a['a.php']->chmod(0777);
var_dump($a['a.php']->isExecutable());
$a['a.php']->chmod(0666);
var_dump($a['a.php']->isExecutable());
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.1.phar.php');
?>
--EXPECTF--
bool(false)
bool(true)
bool(false)
===DONE===
