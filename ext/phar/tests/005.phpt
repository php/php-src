--TEST--
Phar::mapPhar truncated manifest (none)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--FILE--
<?php

try {
    Phar::mapPhar('hio');
} catch (Exception $e) {
    echo $e->getMessage();
}
__halt_compiler(); ?>()

?>
--EXPECTF--
internal corruption of phar "%s" (truncated manifest at manifest length)
