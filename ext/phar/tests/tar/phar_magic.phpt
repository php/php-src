--TEST--
Phar: include/fopen magic tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.tar.php';
$p = new Phar($fname);
var_dump($p->isFileFormat(Phar::TAR));
$p['a'] = '<?php include "b/c.php";' . "\n";
$p['b/c.php'] = '<?php echo "in b\n";$a = fopen("a", "r", true);echo stream_get_contents($a);fclose($a);include dirname(__FILE__) . "/../d";';
$p['d'] = "in d\n";
$p->setStub('<?php
set_include_path("phar://" . __FILE__);
include "phar://" . __FILE__ . "/a";
__HALT_COMPILER();');
include $fname;
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar.php');
__HALT_COMPILER();
?>
--EXPECT--
bool(true)
in b
<?php include "b/c.php";
in d
===DONE===
