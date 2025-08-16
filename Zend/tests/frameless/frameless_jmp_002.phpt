--TEST--
Frameless jmp
--FILE--
<?php
namespace Foo;
var_dump(class_exists('\foo'));
?>
--EXPECT--
bool(false)
