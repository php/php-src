--TEST--
GetImageSize()
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
    }
    var_dump($result);
?>
--EXPECT--
array(18) {
  ["test-1pix.bmp"]=>
  array(8) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(6)
    [3]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(24)
    ["mime"]=>
    string(9) "image/bmp"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test12pix.webp"]=>
  array(8) {
    [0]=>
    int(4)
    [1]=>
    int(3)
    [2]=>
    int(18)
    [3]=>
    string(20) "width="4" height="3""
    ["bits"]=>
    int(8)
    ["mime"]=>
    string(10) "image/webp"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test1bpix.bmp"]=>
  array(8) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(6)
    [3]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(32)
    ["mime"]=>
    string(9) "image/bmp"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test1pix.avif"]=>
  array(9) {
    [0]=>
    int(102)
    [1]=>
    int(121)
    [2]=>
    int(19)
    [3]=>
    string(24) "width="102" height="121""
    ["bits"]=>
    int(8)
    ["channels"]=>
    int(4)
    ["mime"]=>
    string(10) "image/avif"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test1pix.bmp"]=>
  array(8) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(6)
    [3]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(24)
    ["mime"]=>
    string(9) "image/bmp"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test1pix.jp2"]=>
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
  ["test1pix.jpc"]=>
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
  ["test1pix.jpg"]=>
  array(9) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    string(20) "width="1" height="1""
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
  ["test2pix.gif"]=>
  array(9) {
    [0]=>
    int(2)
    [1]=>
    int(1)
    [2]=>
    int(1)
    [3]=>
    string(20) "width="2" height="1""
    ["bits"]=>
    int(1)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(9) "image/gif"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test3llpix.webp"]=>
  array(8) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(18)
    [3]=>
    string(20) "width="1" height="3""
    ["bits"]=>
    int(8)
    ["mime"]=>
    string(10) "image/webp"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test3pix.webp"]=>
  array(8) {
    [0]=>
    int(1)
    [1]=>
    int(3)
    [2]=>
    int(18)
    [3]=>
    string(20) "width="1" height="3""
    ["bits"]=>
    int(8)
    ["mime"]=>
    string(10) "image/webp"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test4pix.gif"]=>
  array(9) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(1)
    [3]=>
    string(20) "width="4" height="1""
    ["bits"]=>
    int(2)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(9) "image/gif"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test4pix.heic"]=>
  array(9) {
    [0]=>
    int(54)
    [1]=>
    int(84)
    [2]=>
    int(20)
    [3]=>
    string(22) "width="54" height="84""
    ["bits"]=>
    int(8)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(10) "image/heif"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test4pix.iff"]=>
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
  ["test4pix.png"]=>
  array(8) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(3)
    [3]=>
    string(20) "width="4" height="1""
    ["bits"]=>
    int(4)
    ["mime"]=>
    string(9) "image/png"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test4pix.psd"]=>
  array(7) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(5)
    [3]=>
    string(20) "width="4" height="1""
    ["mime"]=>
    string(9) "image/psd"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test4pix.swf"]=>
  array(7) {
    [0]=>
    int(550)
    [1]=>
    int(400)
    [2]=>
    int(4)
    [3]=>
    string(24) "width="550" height="400""
    ["mime"]=>
    string(29) "application/x-shockwave-flash"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
  ["test4pix.tiff"]=>
  array(7) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(7)
    [3]=>
    string(20) "width="4" height="1""
    ["mime"]=>
    string(10) "image/tiff"
    ["width_unit"]=>
    string(2) "px"
    ["height_unit"]=>
    string(2) "px"
  }
}
