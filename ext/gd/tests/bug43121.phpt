--TEST--
Bug #43121 (gdImageFill with IMG_COLOR_TILED crashes httpd)
--EXTENSIONS--
gd
--SKIPIF--
<?php
    if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.0', '<')) {
        die("skip test requires GD 2.2.0 or higher");
    }
?>
--FILE--
<?php
$im = imagecreate( 200, 100 );
$black = imagecolorallocate( $im, 0, 0, 0 );

$im_tile = imagecreatefromgif(__DIR__ . "/bug43121.gif" );
imagesettile( $im, $im_tile );
imagefill( $im, 0, 0, IMG_COLOR_TILED );

print "OK";
?>
--EXPECT--
OK
