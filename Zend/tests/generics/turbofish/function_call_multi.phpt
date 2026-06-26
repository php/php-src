--TEST--
Turbofish: function call with multiple type arguments
--FILE--
<?php
function combine<A, B>($a, $b) { return [$a, $b]; }
var_dump(combine::<int, string>(1, "x"));
?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(1) "x"
}
