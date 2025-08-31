--TEST--
Phar::mapPhar truncated manifest (not enough for manifest length)
--EXTENSIONS--
phar
--FILE--
<?php
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";
$file .= pack('V', 500) . 'notenough';
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
internal corruption of phar "%s" (truncated manifest header)
