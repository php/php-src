--TEST--
imagefilter() function test
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!function_exists("imagefilter")) die("skip requires imagefilter function");
    if (!(imagetypes() & IMG_PNG)) {
        die("skip No PNG support");
    }
?>
--FILE--
<?php
$no_arg_filters = array(
    "IMG_FILTER_NEGATE",
    "IMG_FILTER_GRAYSCALE",
    "IMG_FILTER_EDGEDETECT",
    "IMG_FILTER_GAUSSIAN_BLUR",
    "IMG_FILTER_SELECTIVE_BLUR",
    "IMG_FILTER_EMBOSS",
    "IMG_FILTER_MEAN_REMOVAL"
);

$SAVE_DIR = __DIR__;
$SOURCE_IMG = $SAVE_DIR . "/test.png";

    foreach ($no_arg_filters as $filt) {
        $im = imagecreatefrompng($SOURCE_IMG);
        if (imagefilter($im, constant($filt))) {
            imagepng($im, $SAVE_DIR."/".$filt. ".png");
            echo "$filt success\n";
            @unlink($SAVE_DIR."/".$filt. ".png");
        } else {
            echo "$filt failed\n";
        }
    }

    $im = imagecreatefrompng($SOURCE_IMG);

    if (imagefilter($im, IMG_FILTER_SMOOTH, -1924.124)) {
        imagepng($im, $SAVE_DIR . "/IMG_FILTER_SMOOTH.png");
        echo "IMG_FILTER_SMOOTH success\n";
        @unlink($SAVE_DIR . "/IMG_FILTER_SMOOTH.png");
    } else {
        echo "IMG_FILTER_SMOOTH failed\n";
    }

    $im = imagecreatefrompng($SOURCE_IMG);

    if (imagefilter($im, IMG_FILTER_COLORIZE, -127, -127, 127)) {
        imagepng($im, $SAVE_DIR . "/IMG_FILTER_COLORIZE.png");
        echo "IMG_FILTER_COLORIZE success\n";
        unlink($SAVE_DIR . "/IMG_FILTER_COLORIZE.png");
    } else {
        echo "IMG_FILTER_COLORIZE failed\n";
    }

    $im = imagecreatefrompng($SOURCE_IMG);

    if (imagefilter($im, IMG_FILTER_CONTRAST, -90)) {
        imagepng($im, $SAVE_DIR . "/IMG_FILTER_CONTRAST.png");
        echo "IMG_FILTER_CONTRAST success\n";
        unlink($SAVE_DIR . "/IMG_FILTER_CONTRAST.png");
    } else {
        echo "IMG_FILTER_CONTRAST failed\n";
    }

    $im = imagecreatefrompng($SOURCE_IMG);

    if (imagefilter($im, IMG_FILTER_BRIGHTNESS, 98)) {
        imagepng($im, $SAVE_DIR . "/IMG_FILTER_BRIGHTNESS.png");
        echo "IMG_FILTER_BRIGHTNESS success\n";
        unlink($SAVE_DIR . "/IMG_FILTER_BRIGHTNESS.png");
    } else {
        echo "IMG_FILTER_BRIGHTNESS failed\n";
    }

    $im = imagecreatefrompng($SOURCE_IMG);

    if (imagefilter($im, IMG_FILTER_PIXELATE, 5, true)) {
        imagepng($im, $SAVE_DIR . "/IMG_FILTER_PIXELATE.png");
        echo "IMG_FILTER_PIXELATE success\n";
        unlink($SAVE_DIR . "/IMG_FILTER_PIXELATE.png");
    } else {
        echo "IMG_FILTER_PIXELATE failed\n";
    }

    $im = imagecreatefrompng($SOURCE_IMG);

    if (imagefilter($im, IMG_FILTER_SCATTER, 3, 5)) {
        imagepng($im, $SAVE_DIR . "/IMG_FILTER_SCATTER.png");
        echo "IMG_FILTER_SCATTER success\n";
        unlink($SAVE_DIR . "/IMG_FILTER_SCATTER.png");
    } else {
        echo "IMG_FILTER_SCATTER failed\n";
    }
?>
--EXPECT--
IMG_FILTER_NEGATE success
IMG_FILTER_GRAYSCALE success
IMG_FILTER_EDGEDETECT success
IMG_FILTER_GAUSSIAN_BLUR success
IMG_FILTER_SELECTIVE_BLUR success
IMG_FILTER_EMBOSS success
IMG_FILTER_MEAN_REMOVAL success
IMG_FILTER_SMOOTH success
IMG_FILTER_COLORIZE success
IMG_FILTER_CONTRAST success
IMG_FILTER_BRIGHTNESS success
IMG_FILTER_PIXELATE success
IMG_FILTER_SCATTER success
