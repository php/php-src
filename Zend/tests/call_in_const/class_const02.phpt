--TEST--
Can call internal functions from class constants
--FILE--
<?php
class Example {
    const X = [
        'key1' => 'value1',
        'key2' => 'value2',
    ];
    const Y = array_flip(self::X);
    const Z = array_flip(self::Y);
}
var_export(Example::Z);
var_export(Example::Z);
var_export(Example::Y);
?>
--EXPECT--
array (
  'key1' => 'value1',
  'key2' => 'value2',
)array (
  'key1' => 'value1',
  'key2' => 'value2',
)array (
  'value1' => 'key1',
  'value2' => 'key2',
)