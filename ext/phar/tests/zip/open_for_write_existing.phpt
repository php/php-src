--TEST--
Phar: fopen a .phar for writing (existing file) zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip.php';
$pname = 'phar://' . $fname;
$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('.phar/stub.php', "<?php __HALT_COMPILER(); ?>");

$files = array();
$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
foreach ($files as $n => $file) {
$a->addFile($n, $file);
}
$a->close();

$fp = fopen($pname . '/b/c.php', 'wb');
fwrite($fp, 'extra');
fclose($fp);
include $pname . '/b/c.php';
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip.php'); ?>
--EXPECT--
extra
===DONE===
