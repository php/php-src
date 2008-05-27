--TEST--
Bug #13213 (GetImageSize and wrong JPEG Comments)
--FILE--
<?php
var_dump(GetImageSize(dirname(__FILE__).'/bug13213.jpg'));
?>
--EXPECT--
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  unicode(20) "width="1" height="1""
  [u"bits"]=>
  int(8)
  [u"channels"]=>
  int(3)
  [u"mime"]=>
  unicode(10) "image/jpeg"
}
