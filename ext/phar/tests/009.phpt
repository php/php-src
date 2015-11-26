--TEST--
Phar::mapPhar too many manifest entries
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.require_hash=0
--FILE--
<?php
$file = b"<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";
$file .= (binary) pack(b'VVnVVV', 500, 500, 0x1000, 0x00000000, 0, 0) . (binary) str_repeat((binary)'A', 500);
file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
try {
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
} catch (Exception $e) {
echo $e->getMessage();
}
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
internal corruption of phar "%s009.phar.php" (too many manifest entries for size of manifest)
