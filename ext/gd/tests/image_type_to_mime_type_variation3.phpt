--TEST--
Test image_type_to_mime_type() function : usage variations  - Pass equivalent imagetype constant integer values
--FILE--
<?php
/* Prototype  : string image_type_to_mime_type(int imagetype)
 * Description: Get Mime-Type for image-type returned by getimagesize, exif_read_data, exif_thumbnail, exif_imagetype 
 * Source code: ext/standard/image.c
 */

echo "*** Testing image_type_to_mime_type() : usage variations ***\n";

//There are 17 imagetypes
$number_of_imagetypes = 17;

$iterator = 1;
for($imagetype = 1; $imagetype<=$number_of_imagetypes; $imagetype++) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( image_type_to_mime_type($imagetype) );
  $iterator++;
}
?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing image_type_to_mime_type\(\) : usage variations \*\*\*

-- Iteration 1 --
string\(9\) "image\/gif"

-- Iteration 2 --
string\(10\) "image\/jpeg"

-- Iteration 3 --
string\(9\) "image\/png"

-- Iteration 4 --
string\(29\) "application\/x-shockwave-flash"

-- Iteration 5 --
string\(9\) "image\/psd"

-- Iteration 6 --
string\(14\) "image\/x-ms-bmp"

-- Iteration 7 --
string\(10\) "image\/tiff"

-- Iteration 8 --
string\(10\) "image\/tiff"

-- Iteration 9 --
string\(24\) "application\/octet-stream"

-- Iteration 10 --
string\(9\) "image\/jp2"

-- Iteration 11 --
string\(24\) "application\/octet-stream"

-- Iteration 12 --
string\(24\) "application\/octet-stream"

-- Iteration 13 --
string\(2[49]\) "application\/(x-shockwave-flash|octet-stream)"

-- Iteration 14 --
string\(9\) "image\/iff"

-- Iteration 15 --
string\(18\) "image\/vnd.wap.wbmp"

-- Iteration 16 --
string\(9\) "image\/xbm"

-- Iteration 17 --
string\(24\) "image\/vnd.microsoft.icon"
===DONE===
