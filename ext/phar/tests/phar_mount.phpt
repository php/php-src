--TEST--
Phar: Phar::mount
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
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tempmanifest1.phar.php');
?>
--EXPECTF--
Mounting of testit to %sphar_mount.php within phar %stempmanifest1.phar.php failed
Can only mount internal paths within a phar archive, use a relative path instead of "phar://%stempmanifest1.phar.php/testit1"
===DONE===