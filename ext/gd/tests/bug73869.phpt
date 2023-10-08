--TEST--
Bug #73869 (Signed Integer Overflow gd_io.c)
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
var_dump(imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . 'bug73869a.gd2'));
var_dump(imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . 'bug73869b.gd2'));
?>
--EXPECTF--
Warning: imagecreatefromgd2(): "%s" is not a valid GD2 file in %s on line %d
bool(false)

Warning: imagecreatefromgd2(): "%s" is not a valid GD2 file in %s on line %d
bool(false)
