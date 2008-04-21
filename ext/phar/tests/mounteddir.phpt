--TEST--
Phar: mounted manifest directory test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/tempmanifest1.phar.php';
$pname = 'phar://' . $fname;

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

$a = opendir($pname . '/testit');
$out = array();
while (false !== ($b = readdir($a))) {
	$out[] = $b;
}
sort($out);
foreach ($out as $b) {
	echo "$b\n";
}
$out = array();
foreach (new Phar($pname . '/testit') as $b) {
	$out[] = $b->getPathName();
}
sort($out);
foreach ($out as $b) {
	echo "$b\n";
}
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
.
..
extfile.php
extfile2.php
phar://%stempmanifest1.phar.php/testit%cextfile.php
phar://%stempmanifest1.phar.php/testit%cextfile2.php
===DONE===