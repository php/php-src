--TEST--
Test getimagesize() function : basic functionality
--FILE--
<?php
/* Prototype  : array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array 
 * Source code: ext/standard/image.c
 */

$imagetype_filenames = array(
      // GIF file
      "GIF image file" => "200x100.GIF",

      //JPEG file
      "JPEG image file" => "200x100.JPG",

      //PNG file
      "PNG image file" => "200x100.PNG",
    
      //SWF file
      "SWF image file" => "200x100.swf",
      
      //BMP file
      "BMP image file" => "200x100.bmp",
      
      //TIFF intel byte order
      "TIFF intel byte order image file" => "200x100.TIF",

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
      var_dump( getimagesize(dirname(__FILE__)."/$filename", $info) );
      var_dump( $info );
};
?>
===DONE===
--EXPECTF--
*** Testing getimagesize() : basic functionality ***

-- GIF image file (200x100.GIF) --
array(7) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(1)
  [3]=>
  unicode(24) "width="200" height="100""
  [u"bits"]=>
  int(8)
  [u"channels"]=>
  int(3)
  [u"mime"]=>
  unicode(9) "image/gif"
}
array(0) {
}

-- JPEG image file (200x100.JPG) --
array(7) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(2)
  [3]=>
  unicode(24) "width="200" height="100""
  [u"bits"]=>
  int(8)
  [u"channels"]=>
  int(3)
  [u"mime"]=>
  unicode(10) "image/jpeg"
}
array(1) {
  [u"APP0"]=>
  unicode(14)%sJFIF%s
}

-- PNG image file (200x100.PNG) --
array(6) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(3)
  [3]=>
  unicode(24) "width="200" height="100""
  [u"bits"]=>
  int(8)
  [u"mime"]=>
  unicode(9) "image/png"
}
array(0) {
}

-- SWF image file (200x100.swf) --
array(5) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(4)
  [3]=>
  unicode(24) "width="200" height="100""
  [u"mime"]=>
  unicode(29) "application/x-shockwave-flash"
}
array(0) {
}

-- BMP image file (200x100.bmp) --
array(6) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(6)
  [3]=>
  unicode(24) "width="200" height="100""
  [u"bits"]=>
  int(24)
  [u"mime"]=>
  unicode(9) "image/bmp"
}
array(0) {
}

-- TIFF intel byte order image file (200x100.TIF) --
array(5) {
  [0]=>
  int(200)
  [1]=>
  int(100)
  [2]=>
  int(7)
  [3]=>
  unicode(24) "width="200" height="100""
  [u"mime"]=>
  unicode(10) "image/tiff"
}
array(0) {
}

-- JPC image file (test1pix.jpc) --
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(9)
  [3]=>
  unicode(20) "width="1" height="1""
  [u"bits"]=>
  int(8)
  [u"channels"]=>
  int(3)
  [u"mime"]=>
  unicode(24) "application/octet-stream"
}
array(0) {
}

-- JP2 image file (test1pix.jp2) --
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(10)
  [3]=>
  unicode(20) "width="1" height="1""
  [u"bits"]=>
  int(8)
  [u"channels"]=>
  int(3)
  [u"mime"]=>
  unicode(9) "image/jp2"
}
array(0) {
}

-- IFF image file (test4pix.iff) --
array(6) {
  [0]=>
  int(4)
  [1]=>
  int(1)
  [2]=>
  int(14)
  [3]=>
  unicode(20) "width="4" height="1""
  [u"bits"]=>
  int(4)
  [u"mime"]=>
  unicode(9) "image/iff"
}
array(0) {
}
===DONE===