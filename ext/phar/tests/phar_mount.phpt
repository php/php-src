--TEST--
Phar: Phar::mount
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.tar';

$a = new Phar($fname);
$a['index.php'] = '<?php
Phar::mount("testit", "' . addslashes(__FILE__) . '");
try {
Phar::mount("testit", "' . addslashes(__FILE__) . '");
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
Phar::mount("' . addslashes($pname) . '/testit1", "' . addslashes(__FILE__) . '");
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>';
$a->setStub('<?php
set_include_path("phar://" . __FILE__);
include "index.php";
__HALT_COMPILER();');
Phar::mount($pname . '/testit1', __FILE__);
include $fname;
// test copying of a phar with mounted entries
$b = $a->convertToExecutable(Phar::TAR);
$b->setStub('<?php
set_include_path("phar://" . __FILE__);
include "index.php";
__HALT_COMPILER();');
try {
include $fname2;
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
try {
Phar::mount($pname . '/oops', '/home/oops/../../etc/passwd:');
} catch (Exception $e) {
echo $e->getMessage(),"\n";
}
Phar::mount($pname . '/testit2', $pname . '/testit1');
echo substr($a['testit2']->getContent(),0, 50),"\n";
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--
Mounting of testit to %sphar_mount.php within phar %sphar_mount.phar.php failed
Can only mount internal paths within a phar archive, use a relative path instead of "phar://%sphar_mount.phar.php/testit1"
Mounting of testit to %sphar_mount.php within phar %sphar_mount.phar.tar failed
Mounting of /oops to /home/oops/../../etc/passwd: within phar %sphar_mount.phar.php failed
<?php
$fname = dirname(__FILE__) . '/' . basename(
===DONE===