--TEST--
Phar::mapPhar too many manifest entries
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
$file = "<?php
Phar::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>";
$file .= pack('VV', 500, 500) . str_repeat('A', 500);
file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Fatal error: Phar::mapPhar(): too many manifest entries for size of manifest in phar "%s" in %s on line %d