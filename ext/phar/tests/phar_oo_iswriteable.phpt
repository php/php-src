--TEST--
Phar::isWriteable
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
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
$files['dir/'] = '';
$hasdir = 1;
include 'files/phar_test.inc';
$a = new Phar($fname);
var_dump($a['a.php']->isWritable());
var_dump($a['a.php']->isReadable());
$a['a.php']->chmod(000);
var_dump($a['a.php']->isWritable());
var_dump($a['a.php']->isReadable());
$a['a.php']->chmod(0666);
var_dump($a['a.php']->isWritable());
var_dump($a['a.php']->isReadable());
ini_set('phar.readonly',1);
clearstatcache();
var_dump($a['a.php']->isWritable());
var_dump($a['a.php']->isReadable());
ini_set('phar.readonly',0);
clearstatcache();
var_dump($a['a.php']->isWritable());
var_dump($a['a.php']->isReadable());
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.1.phar.php');
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
===DONE===
