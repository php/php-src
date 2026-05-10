--TEST--
Turbofish: first-class callable on function
--FILE--
<?php
function id<T>($x) { return $x; }
$cl = id::<int>(...);
var_dump($cl(7));
?>
--EXPECT--
int(7)
