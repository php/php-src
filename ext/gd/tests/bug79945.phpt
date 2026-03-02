--TEST--
Bug #79945 (using php wrappers in imagecreatefrompng causes segmentation fault)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
set_error_handler(function($errno, $errstr) {
    if (str_contains($errstr, 'Cannot cast a filtered stream on this system')) {
        die('skip: fopencookie not support on this system');
    }
});
imagecreatefrompng('php://filter/read=convert.base64-encode/resource=' . __DIR__ . '/test.png');
restore_error_handler();
?>
--FILE--
<?php
imagecreatefrompng('php://filter/read=convert.base64-encode/resource=' . __DIR__ . '/test.png');
?>
--CLEAN--
--EXPECTF--

Warning: imagecreatefrompng(): "php://filter/read=convert.base64-encode/resource=%s" is not a valid PNG file in %s on line %d
