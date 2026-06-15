--TEST--
Scalar methods: a (string) cast is a valid scalar-method receiver
--FILE--
<?php
$n = 42;
var_dump(((string)$n)->upper());
// Chaining also works off a (string) cast receiver.
$s = "  Mixed  ";
var_dump(((string)$s)->trim()->lower());
?>
--EXPECT--
string(2) "42"
string(5) "mixed"
