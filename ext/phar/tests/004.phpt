--TEST--
Phar::mapPhar no __HALT_COMPILER();
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--FILE--
<?php
try {
Phar::mapPhar('hio');
} catch (Exception $e) {
echo $e->getMessage();
}
?>
--EXPECTF--
__HALT_COMPILER(); must be declared in a phar
