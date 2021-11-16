--TEST--
bcadd() incorrect argument count
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=-2
--FILE--
<?php
echo bcadd("-4.27", "7.3");
?>
--EXPECT--
3
