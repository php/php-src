--TEST--
Phar with meta-data (Read)
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
// file length is too short

$files = array();
$files['c'] = array('*', '*',                                           0x00000000);
$manifest = '';
foreach($files as $name => $cont) {
	$ulen = strlen($cont[0]);
	$clen = strlen($cont[1]);
	$manifest .= pack('V', strlen($name)) . $name 
	          . pack('VVVVVVv', $ulen, time(), $clen, crc32($cont[0]), $cont[2], 1, strlen('hi there')) . 'hi there' .
	            pack('V', 0);
}
$alias = 'hio';
$manifest = pack('VnVV', count($files), 0x0900, 0x00001000, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont[1];
}

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
var_dump(file_get_contents('phar://hio/c'));
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
string(1) "*"