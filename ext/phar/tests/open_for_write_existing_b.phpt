--TEST--
Phar: fopen a .phar for writing (existing file)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.readonly=1
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
include 'phar_test.inc';

$fp = fopen($pname . '/b/c.php', 'wb');
fwrite($fp, 'extra');
fclose($fp);
include $pname . '/b/c.php';
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--

Warning: include(phar://%sopen_for_write_existing_b.phar.php/b/c.php): failed to open stream: phar error: internal corruption of phar "%sopen_for_write_existing_b.phar.php" (crc32 mismatch on file "b/c.php") in %sopen_for_write_existing_b.php on line %d

Warning: include(): Failed opening 'phar://%sopen_for_write_existing_b.phar.php/b/c.php' for inclusion (include_path='%s') in %sopen_for_write_existing_b.php on line %d
===DONE===
