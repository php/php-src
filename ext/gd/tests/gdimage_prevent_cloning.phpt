--TEST--
Checks that GdImage instances cannot be cloned
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
?>
--FILE--
<?php

    $img_src = imagecreatetruecolor(32, 32);
    $img_dst = clone $img_src;

?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class GdImage in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
