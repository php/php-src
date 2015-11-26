--TEST--
Bug #43121 (gdImageFill with IMG_COLOR_TILED crashes httpd)
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$im = ImageCreate( 200, 100 );
$black = ImageColorAllocate( $im, 0, 0, 0 );

$im_tile = ImageCreateFromGif(dirname(__FILE__) . "/bug43121.gif" );
ImageSetTile( $im, $im_tile );
ImageFill( $im, 0, 0, IMG_COLOR_TILED );

ImageDestroy( $im );

print "OK";
?>
--EXPECTF--
OK
