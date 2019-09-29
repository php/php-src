--TEST--
Phar: mounted manifest directory test
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--CONFLICTS--
tempmanifest1.phar.php
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/tempmanifest1.phar.php';
$pname = 'phar://' . $fname;

$a = new Phar($fname);
$a['index.php'] = '<?php
Phar::mount("testit", dirname(Phar::running(0)) . "/testit");
echo file_get_contents(Phar::running(1) . "/testit/extfile.php"), "\n";
echo file_get_contents(Phar::running(1) . "/testit/directory"), "\n";
echo file_get_contents(Phar::running(1) . "/testit/existing.txt"), "\n";
include "testit/extfile.php";
include "testit/extfile2.php";
try {
Phar::mount(".phar/stub.php", dirname(Phar::running(0)) . "/testit/extfile.php");
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
?>';
$a['testit/existing.txt'] = 'oops';
$a->setStub('<?php
set_include_path("phar://" . __FILE__);
include "index.php";
__HALT_COMPILER();');
unset($a);
mkdir(__DIR__ . '/testit');
mkdir(__DIR__ . '/testit/directory');
file_put_contents(__DIR__ . '/testit/extfile.php', '<?php
var_dump(__FILE__);
?>');
file_put_contents(__DIR__ . '/testit/extfile2.php', '<?php
var_dump(__FILE__);
?>');
include __DIR__ . '/testit/extfile.php';
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
try {
Phar::mount($pname . '/testit', 'another\\..\\mistake');
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
try {
Phar::mount($pname . '/notfound', __DIR__ . '/this/does/not/exist');
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
try {
Phar::mount($pname . '/testit', __DIR__);
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
try {
Phar::mount($pname . '/testit/extfile.php', __DIR__);
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
?>
===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . '/tempmanifest1.phar.php');
@unlink(__DIR__ . '/testit/extfile.php');
@unlink(__DIR__ . '/testit/extfile2.php');
@rmdir(__DIR__ . '/testit/directory');
@rmdir(__DIR__ . '/testit');

?>
--EXPECTF--
string(%d) "%sextfile.php"
<?php
var_dump(__FILE__);
?>

Warning: file_get_contents(phar://%stempmanifest1.phar.php/testit/directory): failed to open stream: phar error: path "testit/directory" is a directory in phar://%stempmanifest1.phar.php/index.php on line %d

oops
string(%d) "phar://%sextfile.php"
string(%d) "phar://%sextfile2.php"
Mounting of .phar/stub.php to %sextfile.php within phar %stests/tempmanifest1.phar.php failed
.
..
directory
extfile.php
extfile2.php
phar://%stempmanifest1.phar.php/testit%cdirectory
phar://%stempmanifest1.phar.php/testit%cextfile.php
phar://%stempmanifest1.phar.php/testit%cextfile2.php
Mounting of /testit to another\..\mistake within phar %stempmanifest1.phar.php failed
Mounting of /notfound to %stests/this/does/not/exist within phar %stempmanifest1.phar.php failed
Mounting of /testit to %stests within phar %stests/tempmanifest1.phar.php failed
Mounting of /testit/extfile.php to %stests within phar %stests/tempmanifest1.phar.php failed
===DONE===
