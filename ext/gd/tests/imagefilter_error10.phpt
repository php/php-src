--TEST--
Testing wrong parameter resource of EMBOSS in imagefilter() of GD library
--CREDITS--
Guilherme Blanco <guilhermeblanco [at] hotmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$image = tmpfile();

try {
    var_dump(imagefilter($image, IMG_FILTER_EMBOSS));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagefilter(): supplied resource is not a valid Image resource
