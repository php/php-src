--TEST--
Phar: test that refcounting avoids problems with deleting a file tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include dirname(__FILE__) . '/files/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.tar';
$alias = 'phar://' . $fname;

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('.phar/stub.php', "<?php __HALT_COMPILER(); ?>");

$files = array();

$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
$files['.phar/alias.txt'] = 'hio';

foreach ($files as $n => $file) {
	$tar->addFile($n, $file);
}

$tar->close();

$fp = fopen($alias . '/b/c.php', 'wb');
fwrite($fp, "extra");
fclose($fp);
echo "===CLOSE===\n";
$phar = new Phar($fname);
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
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--
===CLOSE===
object(PharFileInfo)#%d (2) {
  [%spathName":%sSplFileInfo":private]=>
  string(%d) "phar://%srefcount1.phar.tar/b/c.php"
  [%sfileName":%sSplFileInfo":private]=>
  string(%d) "c.php"
}
string(5) "extra"
===UNLINK===

Warning: unlink(): phar error: "b/c.php" in phar "%srefcount1.phar.tar", has open file pointers, cannot unlink in %srefcount1.php on line %d
object(PharFileInfo)#%d (2) {
  [%spathName":%sSplFileInfo":private]=>
  string(%d) "phar://%srefcount1.phar.tar/b/c.php"
  [%sfileName":%sSplFileInfo":private]=>
  string(%s) "c.php"
}
string(5) "extra"
extra
===DONE===
