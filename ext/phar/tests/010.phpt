--TEST--
PHP_Archive::mapPhar buffer overrun
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
function cleanup() { unlink(dirname(__FILE__) . '/008_phar.php'); }
register_shutdown_function('cleanup');
$file = "<?php
Phar::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>";
$manifest = pack('V', 1) . 'a' . pack('VVVV', 1, time(), 0, 1);
// this fails because the manifest length does not include the 4-byte "length of manifest" data
$file .= pack('VV', strlen($manifest), 1) . $manifest . pack('VV', crc32('a'), 1) . 'a';
file_put_contents(dirname(__FILE__) . '/008_phar.php', $file);
include dirname(__FILE__) . '/008_phar.php';
echo file_get_contents('phar://hio/a');
?>
--EXPECTF--
Fatal error: Phar::mapPhar(): internal corruption of phar "%s" (buffer overrun) in %s on line %d