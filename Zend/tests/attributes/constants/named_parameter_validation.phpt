--TEST--
Verify processing of named arguments
--DESCRIPTION--
Since attributes on constants do not have arguments checked until runtime,
check that the validation logic still works
--FILE--
<?php

#[MyAttribute(foo: "bar", foo: "baz")]
const EXAMPLE = 'Foo';

?>
--EXPECTF--
Fatal error: Duplicate named parameter $foo in %s on line %d
