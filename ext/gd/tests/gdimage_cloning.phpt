--TEST--
Checks that GdImage instances can be cloned from gd > 2.2.0
--SKIPIF--
<?php if (version_compare(GD_VERSION, '2.2.0', '<')) { die("Skipped: GdImage is not clonable before gd 2.2.0"); } ?>
--EXTENSIONS--
gd
--FILE--
<?php

    $foo = imageCreateTrueColor(32, 32);
    $bar = clone $foo;

    $red = imageColorAllocate($foo, 255, 0, 0);
    imageFill($foo, 0, 0, $red);

    $green = imageColorAllocate($bar, 0, 255, 0);
    imageFill($bar, 0, 0, $green);

    var_dump(imageColorAt($foo, 0, 0) === $red);
    var_dump(imageColorAt($bar, 0, 0) === $green);


?>
--EXPECTF--
bool(true)
bool(true)
