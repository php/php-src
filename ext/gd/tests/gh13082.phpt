--TEST--
GH-13082 - imagefontwidth/height unexpectedly throwing an exception on a valid GdFont object.
--EXTENSIONS--
gd
--FILE--
<?php
    $font = imageloadfont(__DIR__ . "/gh13082.gdf");
    if ($font === false) die("imageloadfont failed");
    if (!($font instanceof GdFont)) die("invalid gd font");

    var_dump(imagefontwidth($font));
    var_dump(imagefontheight($font));
?>
--EXPECT--
int(12)
int(20)
