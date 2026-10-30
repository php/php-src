--TEST--
imagefilter() function test
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (PHP_INT_SIZE != 8) die("skip only for 64 bits platforms");
    if (!function_exists("imagefilter")) die("skip requires imagefilter function");
    if (!(imagetypes() & IMG_PNG)) {
        die("skip No PNG support");
    }
?>
--FILE--
<?php
$SAVE_DIR = __DIR__;
$SOURCE_IMG = $SAVE_DIR . "/test.png";
$im = imagecreatefrompng($SOURCE_IMG);

foreach ([-1, PHP_INT_MAX] as $val) {
        try {
            imagefilter($im, IMG_FILTER_SCATTER, $val, 0);
        } catch (\ValueError $e) {
            echo $e->getMessage() . PHP_EOL;
        }
        try {
            imagefilter($im, IMG_FILTER_SCATTER, 0, $val);
        } catch (\ValueError $e) {
            echo $e->getMessage() . PHP_EOL;
        }
}
?>
--EXPECTF--
imagefilter(): Argument #3 must be between 0 and %d
imagefilter(): Argument #4 must be between 0 and %d
imagefilter(): Argument #3 must be between 0 and %d
imagefilter(): Argument #4 must be between 0 and %d
