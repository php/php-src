--TEST--
Bug #72709 (imagesetstyle() causes OOB read for empty $styles)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(1, 1);

try {
    var_dump(imagesetstyle($im, array()));
}
catch (\Error $ex) {
    echo $ex->getMessage() . "\n";
}

imagesetpixel($im, 0, 0, IMG_COLOR_STYLED);
imagedestroy($im);
?>
====DONE====
--EXPECT--
imagesetstyle(): Argument #2 ($style) cannot be empty
====DONE====
