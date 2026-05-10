--TEST--
Turbofish: nested type arguments
--FILE--
<?php
class B<K, V> {}
function f<T>($x) { return $x; }
var_dump(f::<B<int, string>>(["a", "b"]));
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
