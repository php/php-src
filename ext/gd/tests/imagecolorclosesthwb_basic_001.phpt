--TEST--
Test imagecolorclosesthwb() basic functionality
--SKIPIF--
<?php
    if (!extension_loaded('gd')) {
        die("skip gd extension not available.");
    }
?>
--FILE--
<?php
    // Create the size of image or blank image
    $image = imagecreate(500, 200);

    // Set the background color of image
    $background_color = imagecolorallocate($image, 0, 24, 200);

    // Set the text color of image
    $text_color = imagecolorallocate($image, 255, 255, 255);

    // Function to create image which contains string.
    imagestring($image, 5, 180, 100,  "PHP is awesome", $text_color);
    imagestring($image, 3, 120, 120,  "A test for PHP imagecolorclosesthwb function", $text_color);

    var_dump(imagecolorclosesthwb($image, 0, 115, 152)); // 0
    var_dump(imagecolorclosesthwb($image, 0, 24, 200)); // 0
    var_dump(imagecolorclosesthwb($image, 116, 120, 115)); // 1
    var_dump(imagecolorclosesthwb($image, 50, 0, 90)); // 0

    imagedestroy($image);
?>
--EXPECT--
int(0)
int(0)
int(1)
int(0)
