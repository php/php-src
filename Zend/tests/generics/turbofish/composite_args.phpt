--TEST--
Turbofish: union and intersection type arguments
--FILE--
<?php
function f<T>($x) { return $x; }
var_dump(f::<int|string>(42));
var_dump(f::<int&string>(42));
var_dump(f::<(A&B)|C>(42));
?>
--EXPECT--
int(42)
int(42)
int(42)
