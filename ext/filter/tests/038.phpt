--TEST--
Test scalar, array
--EXTENSIONS--
filter
--FILE--
<?php

$var = 12;
$res = filter_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL));
var_dump($res);

$var = array(12);
$res = filter_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL));
var_dump($res);

$var = 12;
$res = filter_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_FORCE_ARRAY));
var_dump($res);



$var = 12;
$res = filter_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_REQUIRE_ARRAY));
var_dump($res);

$var = array(12);
$res = filter_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_REQUIRE_ARRAY));
var_dump($res);

$var = array(12);
$res = filter_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_FORCE_ARRAY|FILTER_REQUIRE_ARRAY));
var_dump($res);

$var = array(12);
$res = filter_var($var, FILTER_VALIDATE_INT, array('flags'=>FILTER_FLAG_ALLOW_OCTAL|FILTER_FORCE_ARRAY));
var_dump($res);

?>
--EXPECT--
int(12)
bool(false)
array(1) {
  [0]=>
  int(12)
}
bool(false)
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
