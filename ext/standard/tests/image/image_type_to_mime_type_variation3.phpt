--TEST--
image_type_to_mime_type() (passinf equivalent integer values)
--CREDITS--
Sanjay Mantoor <sanjay.mantoor@gmail.com>
--FILE--
<?php
echo "*** Testing image_type_to_mime_type() : usage variations ***\n";

for($imagetype = 0; $imagetype <= IMAGETYPE_COUNT; ++$imagetype) {
  echo "\n-- Iteration $imagetype --\n";
  var_dump(image_type_to_mime_type($imagetype));
}
?>
--EXPECTREGEX--
\*\*\* Testing image_type_to_mime_type\(\) : usage variations \*\*\*

-- Iteration 0 --
string\(24\) "application\/octet-stream"

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
string\(9\) "image\/bmp"

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

-- Iteration 18 --
string\(10\) "image\/webp"

-- Iteration 19 --
string\(10\) "image\/avif"

-- Iteration 20 --
string\(10\) "image\/heif"

-- Iteration 21 --
string\(24\) "application\/octet-stream"
