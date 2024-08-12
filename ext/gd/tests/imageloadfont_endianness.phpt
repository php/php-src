--TEST--
imageloadfont() detects endianness correctly
--EXTENSIONS--
gd
--FILE--
<?php
$file_LE = __DIR__ .  '/imageloadfont_endianness_LE.gdf';
$fp = fopen($file_LE, 'wb');
fwrite($fp,
  "\x01\x00\x00\x00" .  // one char
  "\x20\x00\x00\x00" .  // at position 32 (space)
  "\x04\x00\x00\x00" .  // width = 4
  "\x01\x00\x00\x00" .  // height = 1
  "\x01\x00\x01\x01");  // char: [x xx]
fclose($fp);

$file_BE = __DIR__ .  '/imageloadfont_endianness_BE.gdf';
$fp = fopen($file_BE, 'wb');
fwrite($fp,
  "\x00\x00\x00\x01" .  // one char
  "\x00\x00\x00\x20" .  // at position 32 (space)
  "\x00\x00\x00\x04" .  // width = 4
  "\x00\x00\x00\x01" .  // height = 1
  "\x01\x00\x01\x01");  // char: [x xx]
fclose($fp);

foreach (array("LE" => $file_LE, "BE" => $file_BE) as $key => $file) {
  print "\n=== $key ===\n";
  $font = imageloadfont($file);
  print "width = "; var_dump(imagefontwidth($font));
  print "height = "; var_dump(imagefontheight($font));
  $im = imagecreate(4, 1);
  imagecolorallocate($im, 255, 255, 255);
  imagechar($im, $font, 0, 0, ' ', imagecolorallocate($im, 0, 0, 0));
  print "pixels = "; var_export(array(
    imagecolorat($im, 0, 0),
    imagecolorat($im, 1, 0),
    imagecolorat($im, 2, 0),
    imagecolorat($im, 3, 0)));
  print "\n";
  imagedestroy($im);
}
unlink($file_BE);
unlink($file_LE);
?>
--EXPECT--

=== LE ===
width = int(4)
height = int(1)
pixels = array (
  0 => 1,
  1 => 0,
  2 => 1,
  3 => 1,
)

=== BE ===
width = int(4)
height = int(1)
pixels = array (
  0 => 1,
  1 => 0,
  2 => 1,
  3 => 1,
)
