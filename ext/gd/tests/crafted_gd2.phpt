--TEST--
Test max colors for a gd image.
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.3.3', '>=')) {
        die("skip test requires GD 2.3.2 or older");
    }
?>
--FILE--
<?php
imagecreatefromgd(__DIR__ . '/crafted.gd2');
?>
--EXPECTF--
Warning: imagecreatefromgd(): "%scrafted.gd2" is not a valid GD file in %s on line %d
