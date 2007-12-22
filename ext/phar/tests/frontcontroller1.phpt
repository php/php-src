--TEST--
Phar front controller
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';

$files = array();
$gflags = 0x020000;
$files['a.jpg'] = 'hio';

include 'phar_test.inc';

$a = new Phar($fname);
$a->setStub('<?php
Phar::mapPhar();
include "phar://" . __FILE__ . "/a.jpg";
__HALT_COMPILER();');

include $fname;
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phartmp.php');
__HALT_COMPILER();
?>
--EXPECTHEADERS--
Content-type: image/jpeg
Content-length: 3
--EXPECT--
hio===DONE===
