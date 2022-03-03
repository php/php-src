--TEST--
Phar::mapPhar too many manifest entries
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";
$file .= pack('VVnVVV', 500, 500, 0x1000, 0x00000000, 0, 0) .  str_repeat('A', 500);
file_put_contents(__DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
try {
include __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
} catch (Exception $e) {
echo $e->getMessage();
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
internal corruption of phar "%s009.phar.php" (too many manifest entries for size of manifest)
