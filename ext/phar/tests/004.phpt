--TEST--
Phar::mapPhar no __HALT_COMPILER();
--EXTENSIONS--
phar
--FILE--
<?php
try {
Phar::mapPhar('hio');
} catch (Exception $e) {
echo $e->getMessage();
}
?>
--EXPECT--
__HALT_COMPILER(); must be declared in a phar
