--TEST--
Deprecate using null as array offset
--FILE--
<?php
$arr = ['foo' => 'bar', '' => 'baz'];

echo $arr[null] . "\n";

$arr[null] = 'new_value';
echo $arr[''] . "\n";

var_dump(isset($arr[null]));

unset($arr[null]);
var_dump(isset($arr['']));
?>
--EXPECTF--

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
baz

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
new_value

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
bool(true)
bool(false)
