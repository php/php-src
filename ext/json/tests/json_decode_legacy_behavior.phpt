--TEST--
json_decode() default flags preserve pre-change duplicate-key and object behavior
--FILE--
<?php
// Last key wins for duplicate scalars (assoc and object).
var_export(json_decode('{"a":1,"b":2,"a":3}', true));
echo "\n";
var_export(json_decode('{"a":1,"b":2,"a":3}'));
echo "\n";

// Nested duplicate: entire value replaced, not merged.
var_export(json_decode('{"x":{"a":1,"b":2},"x":{"c":3}}', true));
echo "\n";

// Scalar overwrites prior object value.
var_export(json_decode('{"k":{"inner":1},"k":42}', true));
echo "\n";

// Object overwrites prior scalar value.
var_export(json_decode('{"k":1,"k":{"inner":2}}', true));
echo "\n";

// Array duplicate: last array wins.
var_export(json_decode('{"a":[1,2],"a":[3,4,5]}', true));
echo "\n";

// flags=0 with other legacy flags unchanged.
var_export(json_decode('{"id":10000000000000000000}', true, 512, JSON_BIGINT_AS_STRING));
echo "\n";
?>
--EXPECT--
array (
  'a' => 3,
  'b' => 2,
)
(object) array(
   'a' => 3,
   'b' => 2,
)
array (
  'x' => 
  array (
    'c' => 3,
  ),
)
array (
  'k' => 42,
)
array (
  'k' => 
  array (
    'inner' => 2,
  ),
)
array (
  'a' => 
  array (
    0 => 3,
    1 => 4,
    2 => 5,
  ),
)
array (
  'id' => '10000000000000000000',
)
