--TEST--
PHP_Archive::mapPhar filesize too small in manifest
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
function cleanup() { unlink(dirname(__FILE__) . '/008_phar.php'); }
register_shutdown_function('cleanup');
$file = "<?php
PHP_Archive::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>";
// compressed file length does not include 8 bytes for crc/file length and should
$manifest = pack('V', 1) . 'a' . pack('VVVV', 1, time(), 0, 1);
$file .= pack('VV', strlen($manifest) + 4, 1) . $manifest . pack('VV', crc32('a'), 1) . 'a';
file_put_contents(dirname(__FILE__) . '/008_phar.php', $file);
include dirname(__FILE__) . '/008_phar.php';
echo file_get_contents('phar://hio/a');
?>
--EXPECTF--
Fatal error: PHP_Archive::mapPhar(): internal corruption of phar "%s" (file size in phar is not large enough) in %s on line %d