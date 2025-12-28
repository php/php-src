--TEST--
There should be no namespace fallback when using the defined() function
--FILE--
<?php

namespace Foo;

var_dump(\defined('Foo\PHP_INT_MAX'));

$const = 'Foo\PHP_INT_MAX';
var_dump(\defined($const));

?>
--EXPECT--
bool(false)
bool(false)
