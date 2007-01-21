--TEST--
Phar::mapPhar valid file (gzipped)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";
if (!extension_loaded("zlib")) print "skip zlib not present"; ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = 'a';
$manifest = '';
foreach($files as $name => $cont) {
	$len = strlen($cont);
	$manifest .= pack('V', strlen($name)) . $name . pack('VVVVVV', $len, time(), 3, crc32($cont), 0x00001000, 0);
}
$alias = 'hio';
$manifest = pack('VnVV', count($files), 0x0900, 0x00001000, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= chr(75) . chr(4) . chr(0); // 'a' gzdeflated
}

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
echo file_get_contents('phar://hio/a');
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
a