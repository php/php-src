--TEST--
json_decode() JSON_DUPLICATE_KEY_MERGE merges nested objects and overwrites scalars
--FILE--
<?php
$json = '{"a":1,"b":{"x":1},"b":{"y":2},"a":9}';
var_export(json_decode($json, true, 512, JSON_DUPLICATE_KEY_MERGE));
echo "\n";
$json2 = '{"k":[1,2],"k":[3]}';
var_export(json_decode($json2, true, 512, JSON_DUPLICATE_KEY_MERGE));
echo "\n";
--EXPECT--
array (
  'a' => 9,
  'b' => 
  array (
    'x' => 1,
    'y' => 2,
  ),
)
array (
  'k' => 
  array (
    0 => 1,
    1 => 2,
    2 => 3,
  ),
)
