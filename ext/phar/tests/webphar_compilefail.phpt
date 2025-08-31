--TEST--
Phar: Phar::webPhar, open compiled file fails
--EXTENSIONS--
phar
--INI--
phar.readonly=1
--FILE--
<?php
try {
Phar::webPhar('oopsiedaisy.phar', '/index.php');
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
__HALT_COMPILER();
?>
--EXPECTF--
internal corruption of phar "%swebphar_compilefail.php" (truncated manifest at manifest length)
