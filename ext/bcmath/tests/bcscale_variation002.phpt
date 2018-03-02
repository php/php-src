--TEST--
bcadd() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) die("skip bcmath extension not loaded"); ?>
--INI--
bcmath.scale=-2
--FILE--
<?php
echo bcadd("-4.27", "7.3");
?>
--EXPECT--
3