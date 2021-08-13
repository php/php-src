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
$im = ImageCreate( 200, 100 );
$black = ImageColorAllocate( $im, 0, 0, 0 );

$im_tile = ImageCreateFromGif(__DIR__ . "/bug43121.gif" );
ImageSetTile( $im, $im_tile );
ImageFill( $im, 0, 0, IMG_COLOR_TILED );

ImageDestroy( $im );

print "OK";
?>
--EXPECT--
OK
