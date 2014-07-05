--TEST--
imagepalettetotruecollor must return an error if not a resource is given
--CREDITS--
Carlos André Ferrari <caferrari [at] gmail [dot] com>
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available.");
?>
--FILE--
<?php
imagepalettetotruecolor("bla");
?>
--EXPECTF--
Warning: imagepalettetotruecolor() expects parameter 1 to be resource, string given in %s on line %d
