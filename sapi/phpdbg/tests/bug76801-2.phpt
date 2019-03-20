--TEST--
get_included_files() tests
--FILE--
<?php

var_dump(get_included_files());

include(__DIR__."/bug76801-2.inc");
var_dump(get_included_files());

include_once(__DIR__."/bug76801-2.inc");
var_dump(get_included_files());

include(__DIR__."/bug76801-2.inc");
var_dump(get_included_files());

echo "Done\n";
?>
--EXPECTF--
array(1) {
  [0]=>
  string(%d) "%s"
}
Included
array(2) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
}
array(2) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
}
Included
array(2) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
}
Done
