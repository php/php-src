--TEST--
opendir test - no dir specified at all
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
function cleanup() { unlink(dirname(__FILE__) . '/008_phar.php'); }
register_shutdown_function('cleanup');
$file = "<?php
PHP_Archive::mapPhar('hio', true);
__HALT_COMPILER(); ?>";
// file length is too short
$manifest = pack('V', 1) . 'a' . pack('VVVV', 1, time(), 0, 9);
$file .= pack('VV', strlen($manifest) + 4, 1) . $manifest . pack('VV', crc32('a'), 1) . 'a';
file_put_contents(dirname(__FILE__) . '/008_phar.php', $file);
include dirname(__FILE__) . '/008_phar.php';
$dir = opendir('phar://hio');
?>
--EXPECTF--
Warning: opendir(phar://hio): failed to open dir: phar error: no directory in "phar://hio", must have at least phar://hio/ for root directory in %s on line %d