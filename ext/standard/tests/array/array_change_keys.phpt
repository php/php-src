--TEST--
Test array_change_keys() function
--FILE--
<?php
$arrayWithIntKeys = ['foo', 'bar'];
$arrayWithStringKeys = ['foo' => 42, 'bar' => 3];
$arrayWithMixedKeys = ['foo' => 42, 'bar'];

echo "---Testing an empty array---\n";
var_dump(array_change_keys([], function(){}));

echo "---Numeric keys to numeric keys---\n";
var_dump(array_change_keys($arrayWithIntKeys, function($k, $v) {
    return $k * 10;
}));

echo "---Numeric keys to string keys---\n";
var_dump(array_change_keys($arrayWithIntKeys, function($k, $v) {
    return 'test:' . $k;
}));

echo "---String keys to numeric keys---\n";
$i = 0;
var_dump(array_change_keys($arrayWithIntKeys, function($k, $v) use (&$i) {
    return $i++;
}));

echo "---String keys to string keys---\n";
var_dump(array_change_keys($arrayWithStringKeys, function($k, $v) {
    return 'test:' . $k;
}));

echo "---Mixed keys to numeric keys---\n";
$i = 10;
var_dump(array_change_keys($arrayWithMixedKeys, function($k, $v) use (&$i) {
    return $i++;
}));

echo "---Mixed keys to string keys---\n";
var_dump(array_change_keys($arrayWithMixedKeys, function($k, $v) {
    return 'test:' . $k;
}));

echo "---Mixed keys to mixed keys---\n";
var_dump(array_change_keys($arrayWithMixedKeys, function($k, $v) {
    return is_int($k) ? 'baz' : 3;
}));

echo "---Test using a string as the callable---\n";
// We can't reference 'md5' directly because it would get a truthy value in its second param,
// This resulting in some raw bytes instead of a nice human-readable string.
function use_md5_hash_as_key($k, $v) {
    return md5($k);
}
var_dump(array_change_keys($arrayWithMixedKeys, 'use_md5_hash_as_key'));

echo "---Test using the [\$obj, 'method'] callback syntax\n";
$keyMaker = new class {
    function getNewKey($k, $v) {
        return md5($v);
    }
};
var_dump(array_change_keys($arrayWithMixedKeys, [$keyMaker, 'getNewKey']));

?>
--EXPECTF--
---Testing an empty array---
array(0) {
}
---Numeric keys to numeric keys---
array(2) {
  [0]=>
  string(3) "foo"
  [10]=>
  string(3) "bar"
}
---Numeric keys to string keys---
array(2) {
  ["test:0"]=>
  string(3) "foo"
  ["test:1"]=>
  string(3) "bar"
}
---String keys to numeric keys---
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
---String keys to string keys---
array(2) {
  ["test:foo"]=>
  int(42)
  ["test:bar"]=>
  int(3)
}
---Mixed keys to numeric keys---
array(2) {
  [10]=>
  int(42)
  [11]=>
  string(3) "bar"
}
---Mixed keys to string keys---
array(2) {
  ["test:foo"]=>
  int(42)
  ["test:0"]=>
  string(3) "bar"
}
---Mixed keys to mixed keys---
array(2) {
  [3]=>
  int(42)
  ["baz"]=>
  string(3) "bar"
}
---Test using a string as the callable---
array(2) {
  ["acbd18db4cc2f85cedef654fccc4a4d8"]=>
  int(42)
  ["cfcd208495d565ef66e7dff9f98764da"]=>
  string(3) "bar"
}
---Test using the [$obj, 'method'] callback syntax
array(2) {
  ["a1d0c6e83f027327d8461063f4ac58a6"]=>
  int(42)
  ["37b51d194a7513e45b56f6524f2d51f2"]=>
  string(3) "bar"
}