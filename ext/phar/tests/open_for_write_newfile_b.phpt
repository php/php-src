--TEST--
Phar: fopen a .phar for writing (new file)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.readonly=1
phar.require_hash=0
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

$fp = fopen('phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b/new.php', 'wb');
fwrite($fp, 'extra');
fclose($fp);
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b/c.php';
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/b/new.php';
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--

Warning: fopen(phar://%sopen_for_write_newfile_b.phar.php/b/new.php): failed to open stream: phar error: file "b/new.php" could not be created in phar "%sopen_for_write_newfile_b.phar.php" in %sopen_for_write_newfile_b.php on line %d

Warning: fwrite()%sresource%sin %sopen_for_write_newfile_b.php on line %d

Warning: fclose(): supplied argument is not a valid stream resource in %sopen_for_write_newfile_b.php on line %d
This is b/c

Warning: include(phar://%sopen_for_write_newfile_b.phar.php/b/new.php): failed to open stream: phar error: "b/new.php" is not a file in phar "%sopen_for_write_newfile_b.phar.php" in %sopen_for_write_newfile_b.php on line %d

Warning: include(): Failed opening 'phar://%sopen_for_write_newfile_b.phar.php/b/new.php' for inclusion (include_path='.') in %sopen_for_write_newfile_b.php on line %d

===DONE===
