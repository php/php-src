--TEST--
Scale images and preserve aspect ratio
--SKIPIF--
<?php
if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$img = imagecreatetruecolor ( 256, 384);

$thumbnail = imagescale($img, 64, -1, IMG_BICUBIC);
var_dump(imagesx($thumbnail));
var_dump(imagesy($thumbnail));

$thumbnail = imagescale($img, -1, 64, IMG_BICUBIC);
var_dump(imagesx($thumbnail));
var_dump(imagesy($thumbnail));

$img = imagecreatetruecolor ( 384, 256);

$thumbnail = imagescale($img, 64, -1, IMG_BICUBIC);
var_dump(imagesx($thumbnail));
var_dump(imagesy($thumbnail));

$thumbnail = imagescale($img, -1, 64, IMG_BICUBIC);
var_dump(imagesx($thumbnail));
var_dump(imagesy($thumbnail));

$img = imagecreatetruecolor ( 256, 256);

$thumbnail = imagescale($img, 64, -1, IMG_BICUBIC);
var_dump(imagesx($thumbnail));
var_dump(imagesy($thumbnail));

$thumbnail = imagescale($img, -1, 64, IMG_BICUBIC);
var_dump(imagesx($thumbnail));
var_dump(imagesy($thumbnail));

?>
DONE
--EXPECT--
int(64)
int(96)
int(42)
int(64)
int(64)
int(42)
int(96)
int(64)
int(64)
int(64)
int(64)
int(64)
DONE
