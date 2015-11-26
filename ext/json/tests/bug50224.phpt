--TEST--
bug #50224 (json_encode() does not always encode a float as a float)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
echo "* Testing JSON output\n\n";
var_dump(json_encode(12.3, JSON_PRESERVE_ZERO_FRACTION));
var_dump(json_encode(12, JSON_PRESERVE_ZERO_FRACTION));
var_dump(json_encode(12.0, JSON_PRESERVE_ZERO_FRACTION));
var_dump(json_encode(0.0, JSON_PRESERVE_ZERO_FRACTION));
var_dump(json_encode(array(12, 12.0, 12.3), JSON_PRESERVE_ZERO_FRACTION));
var_dump(json_encode((object)array('float' => 12.0, 'integer' => 12), JSON_PRESERVE_ZERO_FRACTION));

echo "\n* Testing encode/decode symmetry\n\n";

var_dump(json_decode(json_encode(12.3, JSON_PRESERVE_ZERO_FRACTION)));
var_dump(json_decode(json_encode(12, JSON_PRESERVE_ZERO_FRACTION)));
var_dump(json_decode(json_encode(12.0, JSON_PRESERVE_ZERO_FRACTION)));
var_dump(json_decode(json_encode(0.0, JSON_PRESERVE_ZERO_FRACTION)));
var_dump(json_decode(json_encode(array(12, 12.0, 12.3), JSON_PRESERVE_ZERO_FRACTION)));
var_dump(json_decode(json_encode((object)array('float' => 12.0, 'integer' => 12), JSON_PRESERVE_ZERO_FRACTION)));
var_dump(json_decode(json_encode((object)array('float' => 12.0, 'integer' => 12), JSON_PRESERVE_ZERO_FRACTION), true));
?>
--EXPECTF--
* Testing JSON output

string(4) "12.3"
string(2) "12"
string(4) "12.0"
string(3) "0.0"
string(14) "[12,12.0,12.3]"
string(27) "{"float":12.0,"integer":12}"

* Testing encode/decode symmetry

float(12.3)
int(12)
float(12)
float(0)
array(3) {
  [0]=>
  int(12)
  [1]=>
  float(12)
  [2]=>
  float(12.3)
}
object(stdClass)#%d (2) {
  ["float"]=>
  float(12)
  ["integer"]=>
  int(12)
}
array(2) {
  ["float"]=>
  float(12)
  ["integer"]=>
  int(12)
}