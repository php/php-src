--TEST--
Frameless jmp
--FILE--
<?php
namespace Foo;
preg_replace('/foo/', '', '');
?>
--EXPECT--
