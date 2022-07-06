--TEST--
Testing class alias with is_subclass_of()
--FILE--
<?php

class foo {
}

class_alias('foo', 'bar');


class baz extends bar {
}

var_dump(is_subclass_of(new foo, 'foo'));
var_dump(is_subclass_of(new foo, 'bar'));
var_dump(is_subclass_of(new foo, 'baz'));

var_dump(is_subclass_of(new bar, 'foo'));
var_dump(is_subclass_of(new bar, 'bar'));
var_dump(is_subclass_of(new bar, 'baz'));

var_dump(is_subclass_of(new baz, 'foo'));
var_dump(is_subclass_of(new baz, 'bar'));
var_dump(is_subclass_of(new baz, 'baz'));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
