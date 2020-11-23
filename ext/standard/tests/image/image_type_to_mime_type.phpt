--TEST--
image_type_to_mime_type()
--SKIPIF--
<?php
    require_once('skipif_imagetype.inc');
?>
--FILE--
<?php
    // Note: SWC requires zlib
    $dir = opendir(__DIR__) or die('cannot open directory: '.__DIR__);
    $result = array();
    $files  = array();
    while (($file = readdir($dir)) !== FALSE) {
        if (preg_match('/^test.+pix\./',$file) && $file != "test13pix.swf") {
            $files[] = $file;
        }
    }
    closedir($dir);
    sort($files);
    foreach($files as $file) {
        $result[$file] = getimagesize(__DIR__."/$file");
        $result[$file] = image_type_to_mime_type($result[$file][2]);
    }
    var_dump($result);
?>
--EXPECT--
array(16) {
  ["test-1pix.bmp"]=>
  string(9) "image/bmp"
  ["test12pix.webp"]=>
  string(10) "image/webp"
  ["test1bpix.bmp"]=>
  string(9) "image/bmp"
  ["test1pix.bmp"]=>
  string(9) "image/bmp"
  ["test1pix.jp2"]=>
  string(9) "image/jp2"
  ["test1pix.jpc"]=>
  string(24) "application/octet-stream"
  ["test1pix.jpg"]=>
  string(10) "image/jpeg"
  ["test2pix.gif"]=>
  string(9) "image/gif"
  ["test3llpix.webp"]=>
  string(10) "image/webp"
  ["test3pix.webp"]=>
  string(10) "image/webp"
  ["test4pix.gif"]=>
  string(9) "image/gif"
  ["test4pix.iff"]=>
  string(9) "image/iff"
  ["test4pix.png"]=>
  string(9) "image/png"
  ["test4pix.psd"]=>
  string(9) "image/psd"
  ["test4pix.swf"]=>
  string(29) "application/x-shockwave-flash"
  ["test4pix.tiff"]=>
  string(10) "image/tiff"
}
