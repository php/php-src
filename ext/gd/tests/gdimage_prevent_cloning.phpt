--TEST--
Checks that GdImage instances cannot be cloned
--SKIPIF--
<?php if (version_compare(GD_VERSION, '2.2.0', '>=')) { die("Skipped: GdImage is clonable from gd 2.2.0"); } ?>
--EXTENSIONS--
gd
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
