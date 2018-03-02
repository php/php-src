--TEST--
bcscale() with negative argument
--SKIPIF--
<?php if(!extension_loaded("bcmath")) die("skip bcmath extension not loaded"); ?>
--INI--
bcmath.scale=0
--FILE--
<?php
bcscale(-4);
echo bcdiv("20.56", "4");
?>
--EXPECT--
5