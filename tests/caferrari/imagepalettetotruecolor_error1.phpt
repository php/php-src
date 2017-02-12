--TEST--
imagepalettetotruecollor must return an error if no resource is given
--CREDITS--
Carlos André Ferrari <caferrari [at] gmail [dot] com>
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available.");
?>
--FILE--
<?php
imagepalettetotruecolor();
?>
--EXPECTF--
Warning: imagepalettetotruecolor() expects exactly 1 parameter, 0 given in %s on line %d
