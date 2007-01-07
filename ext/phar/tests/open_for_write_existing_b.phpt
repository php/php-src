--TEST--
Phar: fopen a .phar for writing (existing file)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.readonly=1
--FILE--
<?php
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a.php'] = '<?php echo "This is a\n"; ?>';
$files['b.php'] = '<?php echo "This is b\n"; ?>';
$files['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
$manifest = '';
foreach($files as $name => $cont) {
	$len = strlen($cont);
	$manifest .= pack('V', strlen($name)) . $name . pack('VVVVC', $len, time(), $len, crc32($cont), 0x00);
}
$alias = '';
$manifest = pack('VnV', count($files), 0x0800, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont;
}

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);

$fp = fopen('phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b/c.php', 'wb');
fwrite($fp, 'extra');
fclose($fp);
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b/c.php';
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--

Warning: include(phar://%sopen_for_write_existing_b.phar.php/b/c.php): failed to open stream: phar error: internal corruption of phar "%sopen_for_write_existing_b.phar.php" (crc32 mismatch on file "b/c.php") in %sopen_for_write_existing_b.php on line %d

Warning: include(): Failed opening 'phar://%sopen_for_write_existing_b.phar.php/b/c.php' for inclusion (include_path='.') in %sopen_for_write_existing_b.php on line %d
===DONE===
