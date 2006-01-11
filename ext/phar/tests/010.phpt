--TEST--
Phar::mapPhar buffer overrun
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

// this fails because the manifest length does not include the other 10 byte manifest data

$manifest = pack('V', 1) . 'a' . pack('VVVVC', 0, time(), 0, crc32(''), 0);
$file .= pack('VVnV', strlen($manifest), 1, 0x0800, 3) . 'hio' . $manifest;

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
echo file_get_contents('phar://hio/a');
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Fatal error: Phar::mapPhar(): internal corruption of phar "%s" (too many manifest entries for size of manifest) in %s on line %d