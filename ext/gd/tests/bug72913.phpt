--TEST--
Bug #72913 (imagecopy() loses single-color transparency on palette images)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php
$base64 = 'iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAIAAACRXR/mAAAABnRSTlMAAAAAAABu'
    . 'pgeRAAAAVklEQVRYw+3UQQqAMBAEwf3/p9eTBxEPiWAmWMU8oGFJqgAAuOpzWTX3'
    . 'xQUti+uRJTZ9V5aY1bOTFZLV7yZr9zt6ibv/qPXfrMpsGipbIy7oqQ8AYJED1plD'
    . 'y5PCu2sAAAAASUVORK5CYII=';
$src = imagecreatefromstring(base64_decode($base64));

$dst = imagecreate(50, 50);
$transparent = imagecolorallocatealpha($dst, 255, 255, 255, 127);
imagealphablending($dst, false);
imagesavealpha($dst, true);

imagecopy($dst, $src, 0,0, 0,0, 50,50);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/bug72913.png', $dst);
?>
--EXPECT--
The images are equal.
