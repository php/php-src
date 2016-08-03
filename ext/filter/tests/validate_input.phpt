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
try {
	$ret = validate_input(INPUT_GET, 'a', FILTER_VALIDATE_INT);
} catch (Exception $e) {
	var_dump($e->getMessage());
}
var_dump($ret);

try {
	$ret = validate_input(INPUT_GET, 'a', FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
var_dump($ret);

try {
	$ret = validate_input(INPUT_GET, 'ar', FILTER_VALIDATE_INT, array('flags'=>FILTER_REQUIRE_ARRAY));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
var_dump($ret);

try {
	$ret = validate_input(INPUT_GET, 'ar', FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_REQUIRE_ARRAY));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
var_dump($ret);

?>
--EXPECTF--
string(28) "Int validation: Out of range"

Notice: Undefined variable: ret in %s on line %d
NULL
int(156)
string(28) "Int validation: Out of range"
int(156)
array(2) {
  ["elm1"]=>
  int(1234)
  ["elm2"]=>
  int(432)
}