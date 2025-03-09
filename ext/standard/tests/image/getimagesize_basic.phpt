--TEST--
Test getimagesize() function : basic functionality
--FILE--
<?php
$imagetype_filenames = array(
      // GIF file
      "GIF image file" => "200x100.gif",

      //JPEG file
      "JPEG image file" => "200x100.jpg",

      //PNG file
      "PNG image file" => "200x100.png",

      //SWF file
      "SWF image file" => "200x100.swf",

      //BMP file
      "BMP image file" => "200x100.bmp",

      //TIFF intel byte order
      "TIFF intel byte order image file" => "200x100.tiff",

      //JPC file
      "JPC image file" => "test1pix.jpc",

      //JP2 file
      "JP2 image file" => "test1pix.jp2",

      //IFF file
      "IFF image file" => "test4pix.iff"
);

echo "*** Testing getimagesize() : basic functionality ***\n";

// loop through each element of the array for imagetype
foreach($imagetype_filenames as $key => $filename) {
      echo "\n-- $key ($filename) --\n";
      var_dump( getimagesize(__DIR__."/$filename", $info) );
      var_dump( $info );
};
?>
--EXPECTF--
*** Testing getimagesize() : basic functionality ***

-- GIF image file (200x100.gif) --
array(9) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(1)
  [3]=>
  string(24) "width="200" height="100""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(9) "image/gif"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}

-- JPEG image file (200x100.jpg) --
array(9) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(2)
  [3]=>
  string(24) "width="200" height="100""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(10) "image/jpeg"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(1) {
  ["APP0"]=>
  string(%d)%s
}

-- PNG image file (200x100.png) --
array(8) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(3)
  [3]=>
  string(24) "width="200" height="100""
  ["bits"]=>
  int(8)
  ["mime"]=>
  string(9) "image/png"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}

-- SWF image file (200x100.swf) --
array(7) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(4)
  [3]=>
  string(24) "width="200" height="100""
  ["mime"]=>
  string(29) "application/x-shockwave-flash"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}

-- BMP image file (200x100.bmp) --
array(8) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(6)
  [3]=>
  string(24) "width="200" height="100""
  ["bits"]=>
  int(24)
  ["mime"]=>
  string(9) "image/bmp"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}

-- TIFF intel byte order image file (200x100.tiff) --
array(7) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(7)
  [3]=>
  string(24) "width="200" height="100""
  ["mime"]=>
  string(10) "image/tiff"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}

-- JPC image file (test1pix.jpc) --
array(9) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(9)
  [3]=>
  string(20) "width="1" height="1""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(24) "application/octet-stream"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}

-- JP2 image file (test1pix.jp2) --
array(9) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(10)
  [3]=>
  string(20) "width="1" height="1""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(9) "image/jp2"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}

-- IFF image file (test4pix.iff) --
array(8) {
  [0]=>
  int(4)
  [1]=>
  int(1)
  [2]=>
  int(14)
  [3]=>
  string(20) "width="4" height="1""
  ["bits"]=>
  int(4)
  ["mime"]=>
  string(9) "image/iff"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(0) {
}
