--TEST--
json_decode() JSON_DUPLICATE_KEY_ARRAY collects duplicate values into a list
--FILE--
<?php
$json = '{"a":1,"a":2,"a":3}';
var_export(json_decode($json, true, 512, JSON_DUPLICATE_KEY_ARRAY));
echo "\n";
$json2 = '{"b":{"x":1},"b":{"y":2}}';
var_export(json_decode($json2, true, 512, JSON_DUPLICATE_KEY_ARRAY));
echo "\n";
--EXPECT--
array (
  'a' => 
  array (
    0 => 1,
    1 => 2,
    2 => 3,
  ),
)
array (
  'b' => 
  array (
    0 => 
    array (
      'x' => 1,
    ),
    1 => 
    array (
      'y' => 2,
    ),
  ),
)
