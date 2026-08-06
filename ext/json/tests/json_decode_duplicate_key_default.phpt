--TEST--
json_decode() duplicate object keys use last-key-wins by default
--FILE--
<?php
$json = '{"a":1,"b":2,"a":3}';
var_export(json_decode($json, true));
echo "\n";
--EXPECT--
array (
  'a' => 3,
  'b' => 2,
)
