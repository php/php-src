--TEST--
imagecreatefromstring() - AVIF format
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_AVIF)) {
    die('skip AVIF support required');
}
?>
--FILE--
<?php
  echo "Reading image whose major brand is 'avif':\n";
  $im = imagecreatefromstring(file_get_contents(__DIR__ . '/imagecreatefromstring_major_brand.avif'));
  var_dump(imagesx($im));
  var_dump(imagesy($im));

  echo "Reading image with a compatible brand that's 'avif':\n";
  $im = imagecreatefromstring(file_get_contents(__DIR__ . '/imagecreatefromstring_compatible_brand.avif'));
  var_dump(imagesx($im));
  var_dump(imagesy($im));
?>
--EXPECT--
Reading image whose major brand is 'avif':
int(250)
int(375)
Reading image with a compatible brand that's 'avif':
int(480)
int(270)
