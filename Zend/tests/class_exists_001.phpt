--TEST--
Testing class_exists() inside namespace
--FILE--
<?php

namespace foo;

class foo {

}

class_alias(__NAMESPACE__ .'\foo', 'bar');


var_dump(class_exists('\bar'));
var_dump(class_exists('bar'));
var_dump(class_exists('foo\bar'));
var_dump(class_exists('foo\foo'));
var_dump(class_exists('foo'));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
