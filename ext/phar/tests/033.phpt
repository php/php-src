--TEST--
Phar::chmod
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.1.phar.php';
$pname = 'phar://hio';
$file = '<?php include "' . $pname . '/a.php"; __HALT_COMPILER(); ?>';

$files = array();
$files['a.php']   = '<?php echo "This is a\n"; include "'.$pname.'/b.php"; ?>';
$files['dir/'] = '';
$hasdir = 1;
include 'files/phar_test.inc';
$a = new Phar($fname);
var_dump($a['a.php']->isExecutable());
$a['a.php']->chmod(0777);
var_dump($a['a.php']->isExecutable());
$a['a.php']->chmod(0666);
var_dump($a['a.php']->isExecutable());
echo "test dir\n";
var_dump($a['dir']->isDir());
var_dump($a['dir']->isReadable());
$a['dir']->chmod(000);
var_dump($a['dir']->isReadable());
$a['dir']->chmod(0666);
var_dump($a['dir']->isReadable());
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.1.phar.php');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
test dir
bool(true)
bool(true)
bool(false)
bool(true)
===DONE===
