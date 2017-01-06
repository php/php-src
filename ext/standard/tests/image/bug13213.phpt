--TEST--
Bug #13213 (GetImageSize and wrong JPEG Comments)
--FILE--
<?php
var_dump(GetImageSize(dirname(__FILE__).'/bug13213.jpg'));
?>
--EXPECTF--
Warning: getimagesize(): corrupt JPEG data: 2 extraneous bytes before marker in %s%ebug13213.php on line %d
array(7) {
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
}
