--TEST--
Phar: temporary manifest entry test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/tempmanifest1.phar.php';
$a = new Phar($fname);
$a['index.php'] = '<?php
set_include_path("' . addslashes(dirname(__FILE__)) . '");
include "extfile.php";
?>';
$a->setStub('<?php
include "phar://" . __FILE__ . "/index.php";
__HALT_COMPILER();');
unset($a);
file_put_contents(dirname(__FILE__) . '/extfile.php', '<?php
var_dump(__FILE__);
?>');
include dirname(__FILE__) . '/extfile.php';
include $fname;
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tempmanifest1.phar.php');
@unlink(dirname(__FILE__) . '/extfile.php');
?>
--EXPECTF--
string(%d) "%sextfile.php"
string(%d) "phar://%sextfile.php"
===DONE===