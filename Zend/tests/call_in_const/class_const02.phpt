--TEST--
Can call internal functions from class constants
--FILE--
<?php
function normalize_keys(array $x) {
    // Should only invoke normalize_keys once
    echo "Normalizing the keys\n";
    $result = [];
    foreach ($x as $k => $value) {
        $result["prefix_$k"] = $value;
    }
    return $result;
}
class Example {
    const X = [
        'key1' => 'value1',
        'key2' => 'value2',
    ];
    const Y = array_flip(self::X);
    const Z = normalize_keys(self::Y);
}
var_export(Example::Z);
var_export(Example::Z);
var_export(Example::Y);
?>
--EXPECT--
Normalizing the keys
array (
  'prefix_value1' => 'key1',
  'prefix_value2' => 'key2',
)array (
  'prefix_value1' => 'key1',
  'prefix_value2' => 'key2',
)array (
  'value1' => 'key1',
  'value2' => 'key2',
)
