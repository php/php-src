--TEST--
RCN check for in-place array modifications
--FILE--
<?php
// Important: do NOT replace range(0, 1) with a variable, these NEED to be TMPVARs!
var_dump(array_replace(range(0, 1), []));
var_dump(array_replace_recursive(range(0, 1), []));
var_dump(array_merge(range(0, 1), []));
var_dump(array_merge_recursive(range(0, 1), []));
var_dump(array_unique(range(0, 1)));
var_dump(array_intersect_ukey(range(0, 1), [], fn () => 0));
var_dump(array_intersect(range(0, 1), []));
var_dump(array_uintersect(range(0, 1), [], fn () => 0));
var_dump(array_intersect_uassoc(range(0, 1), [], fn () => 0));
var_dump(array_uintersect_uassoc(range(0, 1), [], fn () => 0, fn () => 0));
?>
--EXPECT--
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
