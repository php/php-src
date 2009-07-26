--TEST--
mcrypt_list_modes
--SKIPIF--
<?php if (!extension_loaded("mcrypt") || unicode_semantics()) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_list_modes());
--EXPECT--
array(8) {
  [0]=>
  unicode(3) "cbc"
  [1]=>
  unicode(3) "cfb"
  [2]=>
  unicode(3) "ctr"
  [3]=>
  unicode(3) "ecb"
  [4]=>
  unicode(4) "ncfb"
  [5]=>
  unicode(4) "nofb"
  [6]=>
  unicode(3) "ofb"
  [7]=>
  unicode(6) "stream"
}
