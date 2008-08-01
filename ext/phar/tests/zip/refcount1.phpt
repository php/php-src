--TEST--
Phar: test that refcounting avoids problems with deleting a file zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (version_compare(PHP_VERSION, "5.3", "<")) die("skip requires 5.3 or later"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$alias = 'phar://' . $fname;

$phar = new Phar($fname);
$phar->setStub("<?php __HALT_COMPILER(); ?>");
$phar->setAlias('hio');

$files = array();

$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';

foreach ($files as $n => $file) {
	$phar[$n] = $file;
}
$phar->stopBuffering();

$fp = fopen($alias . '/b/c.php', 'wb');
fwrite($fp, b"extra");
fclose($fp);
echo "===CLOSE===\n";
$b = fopen($alias . '/b/c.php', 'rb');
$a = $phar['b/c.php'];
var_dump($a);
var_dump(fread($b, 20));
rewind($b);
echo "===UNLINK===\n";
unlink($alias . '/b/c.php');
var_dump($a);
var_dump(fread($b, 20));
include $alias . '/b/c.php';
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip'); ?>
--EXPECTF--
===CLOSE===
object(PharFileInfo)#%d (2) {
  [%spathName":%sSplFileInfo":private]=>
  string(%d) "phar://%srefcount1.phar.zip/b/c.php"
  [%sfileName":%sSplFileInfo":private]=>
  string(%d) "c.php"
}
string(5) "extra"
===UNLINK===

Warning: unlink(): phar error: "b/c.php" in phar "%srefcount1.phar.zip", has open file pointers, cannot unlink in %srefcount1.php on line %d
object(PharFileInfo)#%d (2) {
  [%spathName":%sSplFileInfo":private]=>
  string(%d) "phar://%srefcount1.phar.zip/b/c.php"
  [%sfileName":%sSplFileInfo":private]=>
  string(%s) "c.php"
}
string(5) "extra"
extra
===DONE===
