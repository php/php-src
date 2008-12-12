--TEST--
Bug #41504 (json_decode() converts empty array keys to "_empty_")
--SKIPIF--
<?php if (!extension_loaded('json')) print 'skip'; ?>
--FILE--
<?php

var_dump(json_decode('{"":"value"}', true));
var_dump(json_decode('{"":"value", "key":"value"}', true));
var_dump(json_decode('{"key":"value", "":"value"}', true));

echo "Done\n";
?>
--EXPECT--	
array(1) {
  [u""]=>
  unicode(5) "value"
}
array(2) {
  [u""]=>
  unicode(5) "value"
  [u"key"]=>
  unicode(5) "value"
}
array(2) {
  [u"key"]=>
  unicode(5) "value"
  [u""]=>
  unicode(5) "value"
}
Done
