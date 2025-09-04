--TEST--
Deprecate using null as key in array_key_exists()
--FILE--
<?php
$arr = ['foo' => 'bar', '' => 'baz'];

var_dump(array_key_exists(null, $arr));
var_dump(array_key_exists('', $arr));
?>
--EXPECTF--
Deprecated: Using null as the key parameter for array_key_exists() is deprecated, use an empty string instead in %s on line %d
bool(true)
bool(true)
