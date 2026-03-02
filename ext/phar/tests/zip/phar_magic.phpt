--TEST--
Phar: include/fopen magic zip-based
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip.php';
$pname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip.php';
$p = new Phar($fname);
var_dump($p->isFileFormat(Phar::ZIP));
$p['a'] = '<?php include "b/c.php";' . "\n";
$p['b/c.php'] = '<?php echo "in b\n";$a = fopen("a", "r", true);echo stream_get_contents($a);fclose($a);include __DIR__ . "/../d";';
$p['d'] = "in d\n";
$p->setStub('<?php
var_dump(__FILE__);
var_dump(substr(__FILE__, 0, 4) != "phar");
set_include_path("phar://" . __FILE__);
include "phar://" . __FILE__ . "/a";
__HALT_COMPILER();');
include $pname;
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip.php');
__HALT_COMPILER();
?>
--EXPECTF--
bool(true)
string(%d) "%sphar_magic.phar.zip.php"
bool(true)
in b
<?php include "b/c.php";
in d
