--TEST--
Phar::mapPhar filesize too small in manifest
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
$file = "<?php
Phar::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>";
// compressed file length does not include 8 bytes for crc/file length and should
$manifest = pack('V', 1) . 'a' . pack('VVVV', 1, time(), 0, 1);
$file .= pack('VV', strlen($manifest) + 4, 1) . $manifest . pack('VV', crc32('a'), 1) . 'a';
file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
echo file_get_contents('phar://hio/a');
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Fatal error: Phar::mapPhar(): internal corruption of phar "%s" (file size in phar is not large enough) in %s on line %d