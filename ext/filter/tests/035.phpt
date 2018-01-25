--TEST--
GET/POST/REQUEST Test with input_filter
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--POST--
d=379
--GET--
ar[elm1]=1234&ar[elm2]=0660&a=0234
--FILE--
<?php
$ret = filter_input(INPUT_GET, 'a', FILTER_VALIDATE_INT);
var_dump($ret);

$ret = filter_input(INPUT_GET, 'a', FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL));
var_dump($ret);

$ret = filter_input(INPUT_GET, 'ar', FILTER_VALIDATE_INT, array('flags'=>FILTER_REQUIRE_ARRAY));
var_dump($ret);

$ret = filter_input(INPUT_GET, 'ar', FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_REQUIRE_ARRAY));
var_dump($ret);

?>
--EXPECT--
bool(false)
int(156)
array(2) {
  ["elm1"]=>
  int(1234)
  ["elm2"]=>
  bool(false)
}
array(2) {
  ["elm1"]=>
  int(1234)
  ["elm2"]=>
  int(432)
}
