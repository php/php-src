--TEST--
Bug #70018 (exec does not strip all whitespace)
--FILE--
<?php
$output = array();
exec('/bin/echo -n -e "abc\f\n \n"',$output);
var_dump($output);
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "abc"
  [1]=>
  string(0) ""
}
