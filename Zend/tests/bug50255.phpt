--TEST--
Bug #50255 (isset() and empty() silently casts array to object)
--FILE--
<?php

$arr = array('foo' => 'bar');

print "isset\n";
var_dump(isset($arr->foo));
var_dump(isset($arr->bar));
var_dump(isset($arr['foo']));
var_dump(isset($arr['bar']));
print "empty\n";
var_dump(empty($arr->foo));
var_dump(empty($arr->bar));
var_dump(empty($arr['foo']));
var_dump(empty($arr['bar']));

?>
--EXPECT--
isset
bool(false)
bool(false)
bool(true)
bool(false)
empty
bool(true)
bool(true)
bool(false)
bool(true)
