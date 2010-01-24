--TEST--
Bug #50732 (exec() adds single byte twice to $output array)
--FILE--
<?php
exec("echo x", $output);
var_dump($output);
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "x"
}
