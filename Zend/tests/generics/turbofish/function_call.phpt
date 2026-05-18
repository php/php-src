--TEST--
Turbofish: function call with single type argument
--FILE--
<?php
function f<T>($x) { return $x; }
var_dump(f::<int>(42));
?>
--EXPECT--
int(42)
