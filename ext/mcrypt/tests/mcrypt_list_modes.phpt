--TEST--
mcrypt_list_modes
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_list_modes());
--EXPECT--
array(8) {
  [0]=>
  string(3) "cbc"
  [1]=>
  string(3) "cfb"
  [2]=>
  string(3) "ctr"
  [3]=>
  string(3) "ecb"
  [4]=>
  string(4) "ncfb"
  [5]=>
  string(4) "nofb"
  [6]=>
  string(3) "ofb"
  [7]=>
  string(6) "stream"
}