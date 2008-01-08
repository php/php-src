--TEST--
Phar: include/fopen magic zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip.php';
$p = new Phar($fname);
var_dump($p->isZip());
$p['a'] = '<?php include "b/c.php";' . "\n";
$p['b/c.php'] = '<?php echo "in b\n";$a = fopen("a", "r", true);echo stream_get_contents($a);fclose($a);include dirname(__FILE__) . "/../d";';
$p['d'] = "in d\n";
$p->setStub('<?php
include "phar://" . __FILE__ . "/a";
__HALT_COMPILER();');
include $fname;
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip.php');
__HALT_COMPILER();
?>
--EXPECTF--
bool(true)
in b
<?php include "b/c.php";
in d
===DONE===
