--TEST--
PHP_Archive::mapPhar too many manifest entries
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
function cleanup() { unlink(dirname(__FILE__) . '/008_phar.php'); }
register_shutdown_function('cleanup');
$file = "<?php
PHP_Archive::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>";
$file .= pack('VV', 500, 500) . str_repeat('A', 500);
file_put_contents(dirname(__FILE__) . '/008_phar.php', $file);
include dirname(__FILE__) . '/008_phar.php';
?>
--EXPECTF--
Fatal error: PHP_Archive::mapPhar(): too many manifest entries for size of manifest in phar "%s" in %s on line %d