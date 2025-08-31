--TEST--
Bug #71912 (libgd: signedness vulnerability)
--EXTENSIONS--
gd
--SKIPIF--
<?php
        if (!GD_BUNDLED) {
            if (version_compare(GD_VERSION, '2.2.0', '<')) {
                die("skip test requires GD 2.2.0 or higher");
            }
            if (version_compare(GD_VERSION, '2.3.3', '>=')) {
                die("skip test requires GD 2.3.2 or older");
            }
        }
        if(!function_exists('imagecreatefromgd2')) die('skip imagecreatefromgd2() not available');
?>
--FILE--
<?php
imagecreatefromgd2(__DIR__ . DIRECTORY_SEPARATOR . "invalid_neg_size私はガラスを食べられます.gd2");
?>
OK
--EXPECTF--
Warning: imagecreatefromgd2(): "%s%einvalid_neg_size私はガラスを食べられます.gd2" is not a valid GD2 file in %s%ebug71912-mb.php on line %d
OK
