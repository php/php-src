--TEST--
Bug #41504 (json_decode() converts empty array keys to "_empty_")
--FILE--
<?php

var_dump(json_decode('{"":"value"}', true));
var_dump(json_decode('{"":"value", "key":"value"}', true));
var_dump(json_decode('{"key":"value", "":"value"}', true));

echo "Done\n";
?>
--EXPECT--
array(1) {
  [""]=>
  string(5) "value"
}
array(2) {
  [""]=>
  string(5) "value"
  ["key"]=>
  string(5) "value"
}
array(2) {
  ["key"]=>
  string(5) "value"
  [""]=>
  string(5) "value"
}
Done
