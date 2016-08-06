--TEST--
Test scalar, array
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

$var = 12;
$res = filter_require_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL));
var_dump($res);

try {
	$var = array(12);
	$res = filter_require_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL));
	var_dump($res);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$var = 12;
$res = filter_require_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_FORCE_ARRAY));
var_dump($res);


try {
	$var = 12;
	$res = filter_require_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_REQUIRE_ARRAY));
	var_dump($res);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

$var = array(12);
$res = filter_require_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_REQUIRE_ARRAY));
var_dump($res);

$var = array(12);
$res = filter_require_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_FORCE_ARRAY|FILTER_REQUIRE_ARRAY));
var_dump($res);

$var = array(12);
$res = filter_require_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_FORCE_ARRAY));
var_dump($res);

?>
--EXPECT--
int(12)
string(52) "Filter validated value is array, but requires scalar"
array(1) {
  [0]=>
  int(12)
}
string(52) "Filter validated value is scalar, but requires array"
array(1) {
  [0]=>
  int(12)
}
array(1) {
  [0]=>
  int(12)
}
array(1) {
  [0]=>
  int(12)
}
