--TEST--
Phar: test that refcounting avoids problems with deleting a file
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
include 'files/phar_test.inc';

$fp = fopen($pname . '/b/c.php', 'wb');
fwrite($fp, "extra");
fclose($fp);
echo "===CLOSE===\n";
$p = new Phar($fname);
$b = fopen($pname . '/b/c.php', 'rb');
$a = $p['b/c.php'];
var_dump($a);
var_dump(fread($b, 20));
rewind($b);
echo "===UNLINK===\n";
unlink($pname . '/b/c.php');
var_dump($a);
var_dump(fread($b, 20));
include $pname . '/b/c.php';
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
===CLOSE===
object(PharFileInfo)#%d (2) {
  [%spathName":%sSplFileInfo":private]=>
  string(%d) "phar://%srefcount1.phar.php/b/c.php"
  [%sfileName":%sSplFileInfo":private]=>
  string(%d) "c.php"
}
string(5) "extra"
===UNLINK===

Warning: unlink(): phar error: "b/c.php" in phar "%srefcount1.phar.php", has open file pointers, cannot unlink in %srefcount1.php on line %d
object(PharFileInfo)#%d (2) {
  [%spathName":%sSplFileInfo":private]=>
  string(%d) "phar://%srefcount1.phar.php/b/c.php"
  [%sfileName":%sSplFileInfo":private]=>
  string(%s) "c.php"
}
string(5) "extra"
extra
===DONE===
