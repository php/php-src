--TEST--
GetImageSize() with 246x247 pixels
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
        if (preg_match('/^246x247\./',$file)) {
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
array(1) {
  ["246x247.png"]=>
  array(6) {
    [0]=>
    int(246)
    [1]=>
    int(247)
    [2]=>
    int(3)
    [3]=>
    string(24) "width="246" height="247""
    ["bits"]=>
    int(4)
    ["mime"]=>
    string(9) "image/png"
  }
}
