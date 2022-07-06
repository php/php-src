--TEST--
Bug #66005 (imagecopy does not support 1bit transparency on truecolor images)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$dest = imagecreatetruecolor(150, 50);
$transparent = imagecolorallocatealpha($dest, 255, 255, 255, 127);
imagealphablending($dest, false);
imagefill($dest, 1, 1, $transparent);
imagesavealpha($dest, true);

// Palette image with transparent color
$png_palette = imagecreatefromstring(base64_decode('iVBORw0KGgoAAAANSUhEUgAAADIAAAAyAgMAAABjUWAiAAAACVBMVEUAAAD/AAD///9nGWQeAAAAAXRSTlMAQObYZgAAAEFJREFUKM9jYBimIASZIxoagOAwhoaGInisQJ4DksJQJKWoPCAnNIQYHsgChBX4eMSbiddlqH5A9R+q39HCZWgDAFxFGyOrmguhAAAAAElFTkSuQmCCPHP'));

// 24 bit with transparent color
$png_24 = imagecreatefromstring(base64_decode('iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAIAAACRXR/mAAAABnRSTlMAAAAAAABupgeRAAAAVklEQVRYw+3UQQqAMBAEwf3/p9eTBxEPiWAmWMU8oGFJqgAAuOpzWTX3xQUti+uRJTZ9V5aY1bOTFZLV7yZr9zt6ibv/qPXfrMpsGipbIy7oqQ8AYJED1plDy5PCu2sAAAAASUVORK5CYII='));

// 32 bit with full alpha channel
$png_full = imagecreatefromstring(base64_decode('iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAYAAAAeP4ixAAAAXklEQVRo3u3XMQrAIBBFwb3/pU2VwiJNIvjdzMD2PlBwqwAAAGajatxz9OGf5viA+KA3EXExXyKiYlqErIiIiBGSFLIyYmuMkO7Xy2MX4ovS/ONoH7Eh/m1nBwCASBe3VYeVaAy8PwAAAABJRU5ErkJggg=='));

imagecopy($dest, $png_palette, 0, 0, 0, 0, 50, 50);
imagecopy($dest, $png_24, 50, 0, 0, 0, 50, 50);
imagecopy($dest, $png_full, 100, 0, 0, 0, 50, 50);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/bug66005.png', $dest);
?>
--EXPECT--
The images are equal.
