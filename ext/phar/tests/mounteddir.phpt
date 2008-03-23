--TEST--
Phar: mounted manifest directory test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/tempmanifest1.phar.php';
$a = new Phar($fname);
$a['index.php'] = '<?php
Phar::mount("testit", dirname(Phar::running(0)) . "/testit");
include "testit/extfile.php";
include "testit/extfile2.php";
?>';
$a->setStub('<?php
set_include_path("phar://" . __FILE__);
include "index.php";
__HALT_COMPILER();');
unset($a);
mkdir(dirname(__FILE__) . '/testit');
file_put_contents(dirname(__FILE__) . '/testit/extfile.php', '<?php
var_dump(__FILE__);
?>');
file_put_contents(dirname(__FILE__) . '/testit/extfile2.php', '<?php
var_dump(__FILE__);
?>');
include dirname(__FILE__) . '/testit/extfile.php';
include $fname;
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tempmanifest1.phar.php');
@unlink(dirname(__FILE__) . '/testit/extfile.php');
@unlink(dirname(__FILE__) . '/testit/extfile2.php');
@rmdir(dirname(__FILE__) . '/testit');

?>
--EXPECTF--
string(%d) "%sextfile.php"
string(%d) "phar://%sextfile.php"
string(%d) "phar://%sextfile2.php"
===DONE===