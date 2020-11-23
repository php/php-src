--TEST--
Allow JSON_OBJECT_AS_ARRAY to have an effect
--FILE--
<?php

$json = '{"foo":"bar"}';

var_dump(json_decode($json, false));
var_dump(json_decode($json, true));
var_dump(json_decode($json, null, 512, 0));
var_dump(json_decode($json, null, 512, JSON_OBJECT_AS_ARRAY));
?>
--EXPECTF--
object(stdClass)#%d (1) {
  ["foo"]=>
  string(3) "bar"
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
object(stdClass)#%d (1) {
  ["foo"]=>
  string(3) "bar"
}
array(1) {
  ["foo"]=>
  string(3) "bar"
}
