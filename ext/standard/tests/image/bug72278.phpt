--TEST--
Bug #72278 (getimagesize returning FALSE on valid jpg)
--FILE--
<?php
define('FILENAME', __DIR__ . DIRECTORY_SEPARATOR . 'bug72278.jpg');

var_dump(getimagesize(FILENAME));
?>
--EXPECTF--
Warning: getimagesize(): Corrupt JPEG data: 3 extraneous bytes before marker in %s%ebug72278.php on line %d
array(7) {
  [0]=>
  int(300)
  [1]=>
  int(300)
  [2]=>
  int(2)
  [3]=>
  string(24) "width="300" height="300""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(10) "image/jpeg"
}
