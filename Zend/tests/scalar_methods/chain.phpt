--TEST--
Scalar methods: chained dispatch on string-returning Str methods
--FILE--
<?php
// The demo: trim then upper, each level desugars to Str::<method>(...).
var_dump("  Hello World  "->trim()->upper());
// Triple chain: trim -> upper -> lower.
var_dump("  abc  "->trim()->upper()->lower());
?>
--EXPECT--
string(11) "HELLO WORLD"
string(3) "abc"
