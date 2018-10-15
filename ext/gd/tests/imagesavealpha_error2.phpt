--TEST--
test imagesavealpha without arguments
--CREDITS--
Marcelo Diniz <marcelo.leo27 [at] gmail [dot] com
‪#‎phpspMaisTestFest PHPSP on 2014-07-05
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available.");
?>
--FILE--
<?php
imagesavealpha();
?>
--EXPECTF--
Warning: imagesavealpha() expects exactly 2 parameters, 0 given in %s on line %d
